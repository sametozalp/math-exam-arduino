#include <LiquidCrystal.h>

#define LED_GREEN 13
#define LED_RED 12

bool number_status = false;

int first_number = 0;
int second_number = 0;

LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

int LCD_END = -1;
int LCD_WRONG_ANSWER = 1;
int LCD_CORRECT_ANSWER = 2;
int LCD_QUESTION = 3;
int LCD_INIT = 4;

int number_of_questions = 0;

void setup()
{
  Serial.begin(9600);
  delay(500);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
}

void loop()
{

  if (Serial.available() > 0)
  {
    int serial = Serial.parseInt();
    Serial.print("Gelen deger: ");
    Serial.println(serial);

    if (serial == LCD_END)
    {
      lcd_end();
    }

    else if (serial == LCD_WRONG_ANSWER)
    {
      number_of_questions += 1;
      Serial.print("number: ");
      Serial.println(number_of_questions);
      lcd_wrong_answer();
    }

    else if (serial == LCD_CORRECT_ANSWER)
    {
      number_of_questions += 1;
      Serial.print("number: ");
      Serial.println(number_of_questions);
      lcd_congratulations();
    }

    else if (serial == LCD_QUESTION)
    {
      delay(1000);
      serial = Serial.parseInt();
      first_number = serial;

      delay(2500);
      serial = Serial.parseInt();
      second_number = serial;

      if (first_number != 0 && second_number != 0)
      {
        lcd_question(first_number, second_number);
      }
    }

    else if (serial == LCD_INIT)
    {
      lcd_init_func();
    }
  }

  delay(2000);

  turn_off_leds();
}

void turn_off_leds()
{
  digitalWrite(LED_RED, false);
  digitalWrite(LED_GREEN, false);
}
// LCD ******************************************

void lcd_init_func()
{

  lcd.begin(16, 2);

  lcd.setCursor(2, 0);
  lcd.print("Welcome to");
  lcd.setCursor(2, 1);
  lcd.print("Math Exam");

  delay(2000);


  lcd.setCursor(2, 0);
  lcd.print("An operation");
  lcd.setCursor(2, 1);
  lcd.print("is expected...");

}

void lcd_question(int &a, int &b)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println(String(a) + " + " + String(b) + " = ?");
  lcd.print(String(a) + " + " + String(b) + " = ?");
}

void lcd_congratulations()
{
  digitalWrite(LED_GREEN, true);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Congratulations");

  delay(2000);

  if (number_of_questions != 5)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Next question");
    lcd.setCursor(3, 1);
    lcd.print("is coming.");
  }
}

void lcd_wrong_answer()
{
  digitalWrite(LED_RED, true);

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Wrong Answer");

  delay(2000);

  if (number_of_questions != 5)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Next question");
    lcd.setCursor(3, 1);
    lcd.print("is coming.");
  }
}

void lcd_end()
{
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("The exam");
  lcd.setCursor(3, 1);
  lcd.print("has ended.");
}
