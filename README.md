# TextAdventureMarkdownLanguage (Taml)
A markdown language for simple text adventures

## How to install
Execute this in your shell to install:
```
git clone https://github.com/SirWolfi/Taml/
```
To build it:
```
cd TextAdventurerMarkdown
g++ Taml.cpp -o Taml.[exe/x86_64]
```
And to run it
```
Taml.[exe/x86_64] <file.taml>
```
_Use .exe on Windows systems and .x86_64 on everything else_
## Syntax
A Taml project is called "an adventure", even tho it hasn't to be one.

### Questions and Answers
Every .taml file is like a questionary <br>
You can declare a question like this:
```
[Question Name]
```
It's possible, but not recomended to use spaces in question names. <br> <br>
Every question has its text which will be printed when the question gets called. <br>
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
_Node: Questions you can jump to can also be variables!_
<br>
There is no limit for questions and answers. <br>
<br>
If the name of a answer is `<>` ` ` or `<empty>` and there is a question to jump to given, it will jump instantly. <br>
If no question is given, the adventure will exit.
When a Question has no answers, the adventure gets stuck once this question is finished. <br>
(This is a feature, exit the adventure with `Strg + C`) <br>
But avoid this please. It's ugly. <br>
<br>
You can make comments with `#`.

### Commands
Commands can be called via `<command>` <br>
A command instruction takes up a whole line. Other text in this line will be ignored! <br>

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
Variables will be carried from question to question! <br>
A not defined variable is an empty string! <br>
<br>
A variable-names can contain these characters. `A-Za-z0-9._:` <br>
That means: <br>
from A to Z, <br>
from a to z, <br>
from 0 to 9, <br>
as well as ".", "_" and ":". <br>
Everything else is not valid!

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
<if (expr) <command>>
```
When `expr` is "True","TRUE","true" or "1", `command` is beeing runned! <br> <br>
An expression looks like:
```
left operator right
```
`left` and `right` can be variables called with `$` or normal strings. <br>
Known operators are `==`, `!=`, `>` and `<`. <br>
`<` and `>` will only work if both values are numbers. If not, an error will be thrown!

#### 4.) else
Executes if the last if failed.
The syntax for `else` is
```
<else <commands>>
```
When the last if failed, `commands` is beeing runned! <br>

#### 5.) clear
Clears the screen on windows and linux. <br>
The syntax for `clear` is
```
<clear>
```

#### 6.) sleep
Stops the interpreter on windows and linux. <br>
The syntax for `sleep` is
```
<sleep MILLISECONDS>
```
Sleeps for `MILLISECONDS`. <br>
Can also be a variable. <br>

#### 7.) jump
Jumps to a question. <br>
The syntax for `jump` is
```
<jump question>
```
`question` can have spaces, but as mentioned before, it's not recomended. <br>

#### 8.) textspeed
Sets the speed at which the text is printed. <br>
The syntax for `textspeed` is
```
<textspeed milliseconds>
```
The default textspeed if 0, that means no delay. <br>
Once a char is printed, the interpeter waites for `milliseconds`. <br>

### Interpreter Variables
Interpreter variables are variables which will be set new for each line interpetered. <br>
They can be used as normal variables, but setting them has no effect.

#### __TEXT_SPEED__
The current text speed.

#### __QUESTION__
The name of the current question the interpreter is in.

#### __QUESTIONS__
The number of total questions.

#### __START__
The first Question at which the interpeter began.

#### __PATH__
The path to the executeable.

#### __ROOT__
The root directory. <br>
On Windows, this might be `C:` or `D:` <br>
On Linux and other Unix-like systems this will probably be `/`

#### __OS__
The operating system you are using. <br>
Either `Windows`, `Linux`, or `Unknwon` (Sorry macOS-user)

## Spacing at beginning
Spaces and tabs at the beginning of a line in the text of questions will be printed. <br>
Questions, answers, includes, comments and commands will ignore these! <br> <br>
That means:
```
# comment
[START]
    # comment
text
{<>}
```
Is the same as
```
        # comment
                [START]
# comment
text
        {<>}
```
(That's not very pretty, but in some cases this can be useful) <br>
Notice that `text` didn't move because every space and tab at the beginning there would be printed! <br>
This is nice when you want for example want to make a layout with tabed in text!

## Including
You can include other adventures like this:
```
%file
```
This ends the last question! <br>
<br>
Be careful: this takes always the path relative to the interpreter run location! <br>
So when you have a file structure like this:
```
Home
| - including
    |- a.taml
    |- b.taml
```
When you run `a.taml` in the `Home` folder, you must include it like this:
```
%including/b.taml
```
When you are in the `including` folder, it has to look like this:
```
%b.taml
```
It is recomended to always run one folder above all seperat files! (In this example in `Home`)

## Style Guide
You can do whatever you want, but here are some suggestions: <br>

### Questions
Question names should be written in `UPPER_SNAKE_CASE`. <br>
So no spaces!

### Variables
Variables should be written `snake_case`. <br>

### Structure
The interpreter begins at the top, so you should build from top to buttom. <br>
The interpreter can jump upwards, but it should be avoided at any point. <br>
The only exception is for loops. <br>

### `{<>}` or `{<empty>}`
Use both as you wish, but I personally like `{<>}` the best.

### `<jumps>`
Use `<jump>` when your jump is based on an if statement. <br>
Don't use `<jump>` like this:
```
[QUESTION]
Hello
<jump OTHER_QUESTION>
{<>}
```
This is the more elegant way:
```
[QUESTION]
Hello
{<>} OTHER_QUESTION
```

### and/or ifs
A logical `and` can be build like this:
```
<if ($a == true) <if ($b == true) <do_something>>>
```
where if `a` AND `b` are true, `do_something` will be executed. <br> <br>
A logical `or` is very simple and looks like this:
```
<if ($a == true) <do_something>>
<if ($b == true) <do_something>>
```
This is the unsave way, because if `a` and `b` are both true `do_something` gets runed twice. <br>
Use this to be on the save side:
```
<if ($a == true) <var tmp is true>>
<if ($b == true) <var tmp is true>>
<if ($tmp == true) <do_something>>
```
This should do it.

### global variables
In Taml, every variable is global, but what I mean here, are variables which are important for the hole adventure. <br>
For example when you store the name of the user. <br> <br>
These variables should be declared at the beginning and be set to `null` or `true`/`false` if they should be a boolean. <br>
`null` is nothing special, just a string, but it shows you that it has no value stored. <br>
This method will help you to keep track of all the important variables AND prevents you from using unset variables. <br> <br>
When you don't need a variable anymore, set it to `null` or just ignore it. Both are valid.

### Naming the Start-Question
When you have more then one file, the first question should have the same name as the file it's in without the extention. <br>
`A` instead of `A.taml` for example. (uppercase of course) <br>
<br>
When it's only one file, you can also name it `BEGIN`, `INIT` `START`, etc

### Comments
Comments should when they are in a question always be tabed in, so you can see them better!

### including
When you include files, they get pasted where you put the include instruction is. So NEVER EVERY put an include above your first question! <br>
Because then the first question of the first file will be used as starting point! <br>
It should look something like this:
```
[EXAMPLE]
    # init variables and stuff
{<>} RUN

%file1
%file2
# ...

[RUN]
    # do things

#...
```

To prevent double definitions of questions, use prefixes like `project::question-name` when you make an adventure for others to build upon. <br>
You don't need that in your own projects, but it's not bad to have.
<br> <br>
It's possible but bad practise to let two files use each others questions. <br>
The better way is to have one main file and other tool files. <br>
_Node: this could lead to unexpected behavior_

## Examples

### demo
You'll find a demo at `Taml/examples/` named `demo.taml`. <br>
<br>
It demonstrates what you can do with Taml.

### including
At `Taml/examples/including` you'll find a short tutorial on how to include and use files/other adventures.

