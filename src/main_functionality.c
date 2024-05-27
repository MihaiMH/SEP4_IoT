#include "includes.h"

// Define the connection parameters
#define SSID "Pixel de Hugo"
#define psswd "Lumia535"
#define address "192.168.114.113"
#define port 88

#define BLOCK_SIZE 16

bool ping_timeout = false;
int timeout_count = 0;
struct AES_ctx my_AES_ctx;
char received_message[128];
uint8_t key[] = "5468617473206D79204B756E67204675";

void init_all()
{
    wifi_init();
    weather_init();
    display_init();
    AES_init_ctx(&my_AES_ctx, key);
}

// Function to pad the input to a multiple of the block size
void pad_input(uint8_t *input, size_t *length)
{
    size_t padded_length = (*length + BLOCK_SIZE - 1) & ~(BLOCK_SIZE - 1);
    memset(input + *length, 0, padded_length - *length);
    *length = padded_length;
}

void AES_ECB_encrypt_buffer(struct AES_ctx *ctx, uint8_t *buf, size_t length)
{
    for (size_t i = 0; i < length; i += BLOCK_SIZE)
    {
        AES_ECB_encrypt(ctx, buf + i);
    }
}

void AES_ECB_decrypt_buffer(struct AES_ctx *ctx, uint8_t *buf, size_t length)
{
    for (size_t i = 0; i < length; i += BLOCK_SIZE)
    {
        AES_ECB_decrypt(ctx, buf + i);
    }
}

void create_and_send_weather()
{
         cJSON *json = cJSON_CreateObject();

        TempHumidLight collectedValues = updateWeather();


        cJSON_AddNumberToObject(json, "temperature", collectedValues.temp);
        cJSON_AddNumberToObject(json, "humidity", collectedValues.humid);
        cJSON_AddNumberToObject(json, "light", collectedValues.light);
        char *jsonString = cJSON_Print(json);
        size_t length = strlen(jsonString);


        pad_input((uint8_t *)jsonString, &length);


        AES_ECB_encrypt_buffer(&my_AES_ctx, (uint8_t *)jsonString, length);

        wifi_command_TCP_transmit((unsigned char *)jsonString, length);

        AES_ECB_decrypt_buffer(&my_AES_ctx, (uint8_t *)jsonString, length);

        cJSON_Delete(json);
        free(jsonString);

        ping_timeout = false;
        timeout_count = 0;
        display_int(5555);
}

void update_data()
{
    if (strcmp(received_message, "updateWeather") == 0)
    {
        create_and_send_weather();
    }
}

WIFI_ERROR_MESSAGE_t wifi_connect()
{
    wifi_command_join_AP(SSID, psswd);
    return wifi_command_create_TCP_connection(address, port, update_data, received_message);
}

void main_function(){
    while (1)
    {
        WIFI_ERROR_MESSAGE_t connection = WIFI_ERROR_NOT_RECEIVING;
        while (connection != WIFI_OK)
        {
            display_int(7777);
            connection = wifi_connect();
            _delay_ms(2000);
        }
        create_and_send_weather();
        display_int(4444);
        ping_timeout = false;
        timeout_count = 0;
        while (!ping_timeout) 
        {
            while (timeout_count < 301) // this is so the updateWeather interrupt resets 30 minute countdown
            {
                display_int(timeout_count);
                _delay_ms(6000);
                timeout_count +=1;
            }
            ping_timeout = true;
        }
        display_int(6666);
        wifi_command_close_TCP_connection();
    }
}