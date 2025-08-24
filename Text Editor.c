#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

struct termios orig_termios;

void die(const char *s)
{
  perror(s); //prints error message
  exit(1);
}

void disableRawMode()
{
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
  die ("tcsetattr");
}

void enableRawMode() 
{

  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1)
  die ("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios; //we store the original terminal attributes in orig_termios
                                     //and we assign it to raw to make a copy of it

  tcgetattr(STDIN_FILENO, &orig_termios);
  raw.c_oflag &= ~(OPOST);
  raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); //ICANON (local flag) to read byte-by-byte
  raw.c_cflag &= ~(CS8);
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
  die("tcsettatr");
}

int main() 
{
  enableRawMode();

  while (1)
  {
    char c='\0';
    if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
    die ("read");
    if (iscntrl(c)) //check if it s a printable ASCII character
    {
      printf("%d\r\n", c);
    } 
    else
    {
      printf("%d (%c)\r\n", c, c); //prints ASCII + input
    }
    if (c == 'q') break;
  }

  return 0;
}