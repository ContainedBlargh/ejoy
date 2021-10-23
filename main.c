#include <stdio.h>
#include <string.h>
#include <orca/discord.h>
#include <stdbool.h>
#include <threads.h>
#include <time.h>

void on_ready(
	struct discord *client,
	const struct discord_user *bot
) {
	log_info("Connected to server as %s!", bot->username);
}

char* stupid_words[4] = {"ej", "jo", "oy", "ye"};

bool content_is_stupid(char* trimmed) {
	int i;
	for (i = 0; i < 4; i++) {
		if (strstr(stupid_words[i], trimmed) != NULL) {
		log_info("Someone wrote something stupid!");
			return true;
		}
	}
	return false;
}

void next(char* m) {
    m[0] ^= m[1];
    m[1] ^= m[0];
    m[0] ^= m[1];
    m[1] = !(((m[1] ^ 0x6a) ^ 0x13) + ((m[1] ^ 0x79) ^ 0x13) ^ 0x13) ? m[1] ^ 0x13 : m[1] ^ 0xa;
}

mtx_t msg_mtx;
u64_snowflake_t conv_partner = 0;

float randf() {
	return (float)rand() / (float)RAND_MAX;
}

char* insults[5] = {
	"dumme bot",
	"bot",
	"kæft",
	"fuck dig",
	"fuck",
};

char* corrections[2] = {
	"jeg bryder mig ikke om udtrykket 'bot', vi foretrækker altså at blive kaldt for 'automater'",
	"hey, det er kun *os* der må bruge b-ordet! >:("
};

char* responses[6] = {
	"tal pænt", //0.4
	"slap lige af", //0.6
	"pas på blodtrykket", //0.8
	"nu synes jeg lige at du skal tælle til 10" //0.9
};


char* insult_response;
bool content_is_insulting(char* content) {
	int i;
	bool insulted = false;
	for (i = 0; i < 5; i++) {
		if (strstr(content, insults[i]) != NULL)
		{
			insulted = true;
			break;
		}
	}
	if (!insulted)
	{
		return false;
	}
	
	float r = randf();
	if (i < 2)
	{
		insult_response = (r < 0.5) ? corrections[0] : corrections[1];
		log_info("Someone wrote something insulting: '%s'!", content);
		return true;
	}
	insult_response = (r <= 0.4) ? responses[0] : 
				(r <= 0.6) ? responses[1] :
				(r <= 0.8) ? responses[2] : responses[3];
	log_info("Someone wrote something insulting: '%s'!", content);
	return true;
}

bool timedout;
time_t until;

void on_message(
	struct discord *client,
	const struct discord_user *bot,
	const struct discord_message *msg
) {
	mtx_lock(&msg_mtx);
	if (timedout)
	{
		time_t now = time(NULL);
		if (now > until)
		{
			log_info("Timeout over!");
			timedout = false;
		}
		mtx_unlock(&msg_mtx);
		return;
	}
	if (msg->author->bot) {
		mtx_unlock(&msg_mtx);
		return;
	}
	if (strstr(msg->content, "!timeout") == msg->content) {
		log_info("Someone asked for a timeout...");
		timedout = true;
		time_t now = time(NULL);
		until = now + 30 * 60;
		mtx_unlock(&msg_mtx);
		return;
	}
	bool same_partner = conv_partner != 0 && conv_partner == msg->author->id;
	conv_partner = msg->author->id;
	char trimmed[3] = {' ', ' ', '\0'};
	trimmed[0] = msg->content[0];
	trimmed[1] = msg->content[1];
	if (content_is_stupid(trimmed)) {
		next(trimmed);
		struct discord_create_message_params params = { .content = trimmed };
		discord_create_message(client, msg->channel_id, &params, NULL);
		mtx_unlock(&msg_mtx);
		return;
	}
	if (same_partner && content_is_insulting(msg->content))
	{
		struct discord_create_message_params params = { .content = insult_response };
		discord_create_message(client, msg->channel_id, &params, NULL);
		mtx_unlock(&msg_mtx);
		return;
	}
	mtx_unlock(&msg_mtx);
}

int main(int argc, char** argv) {
	//Parse token:
	if (argc != 2) {
		printf("Usage: %s <path to discord bot token>\n", argv[0]);
		return 0;
	}
	char token[61];
	FILE *token_file = fopen(argv[1], "r");
	fscanf(token_file, "%s\n", token);	

	//Set initial global state (I know, this is not so nice...)
	insult_response = responses[0];
	srand(time(NULL));
	mtx_init(&msg_mtx, mtx_plain);

	//Connect setup discord client, add handlers and launch.
	struct discord *client = discord_init(token);
	discord_set_on_ready(client, &on_ready);
	discord_set_on_message_create(client, &on_message);

	//This call loops forever:
	discord_run(client);
	return 0;
}
