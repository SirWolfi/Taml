# TextAdventurerMarkdown
A markdown language for simple text adventurers

## How to install
Execute this in your shell to install:
```
git clone https://github.com/SirWolfi/TextAdventurerMarkdown/
```
now to build it use:
```
cd TextAdventurerMarkdown
g++ main.cpp -o TextAdventurerMarkdown.[exe/x84_64]
```
After that you can run it with
```
TextAdventurerMarkdown.[exe/x84_64] <file.tam>
```

## Syntax

### Questions and Answers
Every .tam file is like a questionary <br>
You can declare a question like this:
```
[Question Name]
```
It's possible, but not recomended to use spaces in question names. <br> <br>
Every question has it's text which will be printed when the question gets called. <br>
You can add text like this:
```
[Question]
Here, some nice looking text.
It can handle even new lines oO
Wow another one!
```
A question ends when a new starts:
```
[Question1]
This is the text of Question1

[Question2]
This is the text of Question2

...
```
<br>
Every question has some possible answers. You can declare them like this:
```
[Question]
Text
{Answer1} Question2
{Answer2} Question3
```
When the user chooses `Answer1`, the interpreter jumps to `Question2` <br>
When the user chooses `Answer2`, the interpreter jumps to `Question3` <br>
<br>
There is no limit for questions and answers.

### Commands
Commands can be called via `<command>` <br>
A command instruction takes up a whole line. Other text in this line will be ignored!
#### 1) var
Sets variables. <br>
The syntax for `var` is
```
<var NAME is VALUE>
```
Every variable is a string! <br> <br>
You can call variables in texts using `$NAME`. Then they will be replaced once the interpreter got to it. <br> <br>
For example:
```
<var x is 42>
The answer is $x!
<var x is 27>
3 the power of 3 is $x!
```
Will evaluate to:
```
The answer is 42!
3 the power of 3 is 27!
```
<br>
Variables will be carried from question to question!

#### 2.) input
Takes user inputs. <br>
The syntax for `input` is
```
<input -> NAME>
```
`NAME` is a new variable which stores the user input! <br>

#### 3.) if
Workflow helper.
The syntax for `if` is
```
<if (expr) <commands>>
```
When `expr` is "True","TRUE","true" or "1", `commands` is beeing runned! <br> <br>
An expression looks like:
```
left operator right
```
`left` and `right` can be variables called with `$` or normal strings. <br>
Known operators are `==` and `!=`.

#### 4.) clear
Clears the screen on windows and linux. <br>
The syntax for `clear` is
```
<clear>
```

#### 5.) sleep
Stops the interpreter on windows and linux. <br>
The syntax for `sleep` is
```
<sleep MILLISECONDS>
```
Sleeps for `MILLISECONDS`. <br>
Can also be a variable. <br>

