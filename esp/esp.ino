#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP8266Firebase.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define BOT_TOKEN ""

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

const unsigned long BOT_MTBS = 1000;
unsigned long bot_lasttime;
volatile int result = -1;
volatile int number_of_questions = 0;
volatile int score = 0;
volatile bool block = false;

int LCD_END = -1;
int LCD_WRONG_ANSWER = 1;
int LCD_CORRECT_ANSWER = 2;
int LCD_QUESTION = 3;
int LCD_INIT = 4;

//*************************************************************************************************************
void setup()
{
  Serial.begin(9600);
  delay(500);

  configTime(0, 0, "pool.ntp.org");
  secured_client.setTrustAnchors(&cert);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}
//*************************************************************************************************************
void loop()
{

  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages)
    {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }

  delay(2000);
}

//*************************************************************************************************************
void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;

    if (text == "/start")
    {
      String welcome = "Welcome. " + from_name + "This is a math competition.\n";
      welcome += "The green LED lights up for correct answers, and the red LED lights up for incorrect answers.\n";
      welcome += "If you answer 3 out of 5 questions correctly, you will pass the exam.\n\n";
      welcome += "If you are ready: Start the exam! - /start_exam\n\n";

      welcome += "Good luck.\n\n";

      Serial.println(LCD_INIT);

      bot.sendMessage(chat_id, welcome, "");
    }

    else if (text == "/end_exam")
    {

      if (block == true)
      {
        String exam_is_over = "The exam is over.\n\n";
        exam_is_over += "Your score: " + String(score);

        block = false;
        score = 0;
        number_of_questions = 0;

        Serial.println(LCD_END);

        bot.sendMessage(chat_id, exam_is_over, "");
      }
    }

    else if (text == "/start_exam")
    {
      if (block == false)
      {
        block = true;
        bot.sendMessage(chat_id, "The exam is starting...", "");
        send_question(chat_id);
      }

      else
      {
        bot.sendMessage(chat_id, "Your exam is ongoing.", "");
      }
    }

    else
    {
      if (block == false)
      {
        if (text != "/start")
          bot.sendMessage(chat_id, "I don't understand what you mean.", "");
      }
      else
      {
        if (text == String(result))
        {
          correct_answer(chat_id);
        }
        else
        {
          wrong_answer(chat_id);
        }
      }
    }

    if (number_of_questions == 5)
    {
      String exam_is_over = "The exam is over.\n\n";
      exam_is_over += "Your score: " + String(score);

      if (score < 3)
      {
        exam_is_over += "\nTo pass the exam, you needed to answer " + String(5 - score) + " questions.\n";
        exam_is_over += "\nYou failed the exam.";
      }
      else
      {
        exam_is_over += "\nYou passed the exam, congratulations! Your score is " + String(score) + ".";
      }

      block = false;
      score = 0;
      number_of_questions = 0;

      Serial.println(LCD_END);

      bot.sendMessage(chat_id, exam_is_over, "");
    }
  }
}

void wrong_answer(String &chat_id)
{
  Serial.println(LCD_WRONG_ANSWER);
  String m = "I'm sorry, wrong answer. ";
  m += "Score: ";
  m += String(score);
  bot.sendMessage(chat_id, m, "");

  send_question(chat_id);

  number_of_questions += 1;
}

void correct_answer(String &chat_id)
{
  Serial.println(LCD_CORRECT_ANSWER);
  score += 1;
  String m = "Congratulations, you answered a question correctly.";
  m += " Score: ";
  m += String(score);
  bot.sendMessage(chat_id, m, "");

  send_question(chat_id);

  number_of_questions += 1;
}

void send_question(String &chat_id)
{
  if (number_of_questions < 5)
  {
    Serial.println(LCD_QUESTION);

    delay(1500);

    int first_number = get_number();
    Serial.println(first_number);

    delay(2500);

    int second_number = get_number();
    Serial.println(second_number);

    result = get_res(first_number, second_number);

    if (number_of_questions != 4)
      bot.sendMessage(chat_id, "Answer the question on the screen.", "");
  }
}

int get_res(int &first_number, int &second_number)
{
  return first_number + second_number;
}

int get_number()
{
  return random(20, 100);
}
