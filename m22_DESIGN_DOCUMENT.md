#.m22 DESIGN DOCUMENT
##The specification for the scripting language of the March22 VN engine

The engine parses the next line of the script and does the action based on the first word. This document outlines the functions

| Keywords                                             | What it does                                                                    |
|------------------------------------------------------|---------------------------------------------------------------------------------|
| DrawBackground( _name )                              | Queues specified background to transition to, using active transition           |
| PlayMusic( _name )                                   | Changes active music track to specified track                                   |
| PlaySting( _name )                                   | Plays specified SFX                                                             |
| DrawCharacter ( _name, _outfit, _emotion, _x )       | Draws specified character, in specified outfit/emotion, at position X on-screen |
| ClearCharacters                                      | Clears characters off of active characters array and clears them off-screen     |
| DrawCharacterBrutal ( _name, _outfit, _emotion, _x ) | Same as DrawCharacter, with no transition                                       |
| ClearCharactersBrutal                                | Same as ClearCharacters, with no transition                                     |
| LoadScript ( _filepath )                             | Loads specified script to active script and terminates current one              |
| FadeToBlack                                          | Short-hand for "DrawBackground BLACK"                                           |
| FadeToBlackFancy                                     | Hijacks renderer to fade to black in a nice way                                 |
| StopMusic                                            | Stops any music playing immediately                                             |
| DarkenScreen                                         | Slightly darkens the screen                                                     |
| BrightenScreen                                       | Restores screen brightness from DarkenScreen                                    |
| Goto ( _lineNum )                                    | Goes immediately to the specified line (for debugging)                          |
| PlayLoopedSting ( _name )                            | Plays the specified SFX on loop                                                 |
| StopLoopedStings                                     | Stops all looped SFX                                                            |
| //                                                   | For making comments                                                             |
| ExitGame                                             | Exits game entirely                                                             |
| MainMenu                                             | Goes to main menu                                                               |
| SetActiveTransition ( _name )                        | Sets active transition; "SwipeToRight", "SwipeDown", "SwipeToLeft", "Fade"      |
| Wait ( _milliseconds )                               | Waits the specified milliseconds before moving on                               |
| m22IF ( _decision, _choice, _function_if_true )      | Basic if logic; runs the function if the specified choice is made in the specified decision. |
