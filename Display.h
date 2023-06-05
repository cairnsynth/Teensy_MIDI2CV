#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MidiChannel.h"

#define LINE_HEIGHT 8

class Display : public Adafruit_SSD1306 {
  using Adafruit_SSD1306::Adafruit_SSD1306;
  public:
    void display_channel(MidiChannel channel, int selectedLine) {
      this->setTextSize(1);
      this->setTextColor(WHITE);
      this->clearDisplay();
      this->setCursor(0, 0);
      this->print("<  CH.");
      this->print(channel.get_channel_number());
      this->print("  >");

      if(selectedLine <=2)
      {
        this->setCursor(0, LINE_HEIGHT);
        this->print("Note: ");
        this->print(channel.get_output_note());
        this->setCursor(0, LINE_HEIGHT*2);
        this->print("Gate: ");
        this->print(channel.get_gate());
        this->setCursor(0, LINE_HEIGHT*3);
        this->print("...");
      }

      if(selectedLine > 2)
      {
        this->setCursor(0, LINE_HEIGHT);
        this->print("G Mode: ");
        switch(channel.gateMode)
        {
          case 1:
          {
            this->print("TRIG");
            break;
          }
          default:
          {
            this->print("GATE");
            break;
          }
        }
        this->setCursor(0, LINE_HEIGHT*2);
        this->print("G Leng: ");
        this->print(channel.gateMillis);
      } 
      while(selectedLine > 2)
      {
        selectedLine -= 2;
      }
      for(int x = 0; x < WIDTH; x++)
      {
        for(int y = (selectedLine*LINE_HEIGHT); y < ((selectedLine+1)*LINE_HEIGHT); y++)
        {
          if(!this->getPixel(x, y))
          {
            this->drawPixel(x, y, WHITE);
          }
          else 
          {
            this->drawPixel(x, y, BLACK);
          }
          
        }
      }

      this->display();
    }
    void display_clock(int mult, int selectedLine) {
      this->setTextSize(1);
      this->setTextColor(WHITE);
      this->clearDisplay();
      this->setCursor(0, 0);
      this->print("<  CLOCK  >");
      this->setCursor(0, LINE_HEIGHT);
      this->print("Mult: ");
      switch(mult)
      {
        case 6:
        {
          this->print("1/16");
          break;
        }
        case 8:
        {
          this->print("1/16 T");
          break;
        }
        case 12:
        {
          this->print("1/8");
          break;
        }
        case 16:
        {
          this->print("1/8 T");
          break;
        }
        case 24:
        {
          this->print("1/4");
          break;
        }
        case 32:
        {
          this->print("1/4 T");
          break;
        }
        case 48:
        {
          this->print("1/2");
          break;
        }
        case 64:
        {
          this->print("1/2 T");
          break;
        }
        case 96:
        {
          this->print("1 Bar");
          break;
        }
        case 192:
        {
          this->print("2 Bar");
          break;
        }
        case 384:
        {
          this->print("4 Bar");
          break;
        }
        default:
        {
          this->print(mult);
          this->setCursor(0, 20);
          this->print("MIDI CLK VAL");
        }
      }
      while(selectedLine > 1)
      {
        selectedLine -= 1;
      }
      for(int x = 0; x < WIDTH; x++)
      {
        for(int y = (selectedLine*LINE_HEIGHT); y < ((selectedLine+1)*LINE_HEIGHT); y++)
        {
          if(!this->getPixel(x, y))
          {
            this->drawPixel(x, y, WHITE);
          }
          else 
          {
            this->drawPixel(x, y, BLACK);
          }
          
        }
      }
      this->display();
    }
  private:
};