//Relay testing

void setup()
{
  pinMode (10, OUTPUT); // set pin 2 as an output pin
}

void loop()
{
      digitalWrite (10, HIGH);
    // Bật chân 2 một giây sau đó tắt đi
    delay (10000);
    digitalWrite (10, LOW);
    delay (10000);
 }
