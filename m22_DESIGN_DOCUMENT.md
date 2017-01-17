#.m22 DESIGN DOCUMENT
##The specification for the scripting language of the March22 interactive-novel engine

| Keywords                                             | What it does                                                                    |
|------------------------------------------------------|---------------------------------------------------------------------------------|
| NewPage                                              | Clears current page of text                                                     |
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
| Goto ( _checkpoint )                                 | Goes immediately to the specified checkpoint name (if found)                    |
| PlayLoopedSting ( _name )                            | Plays the specified SFX on loop                                                 |
| StopLoopedStings                                     | Stops all looped SFX                                                            |
| //                                                   | For making comments                                                             |
| ExitGame                                             | Exits game entirely                                                             |
| MainMenu                                             | Goes to main menu                                                               |
| SetActiveTransition ( _name )                        | Sets active transition; "SwipeToRight", "SwipeDown", "SwipeToLeft", "Fade"      |
| Wait ( _milliseconds )                               | Waits the specified milliseconds before moving on                               |
| m22IF ( _decision, _choice, _function_if_true )      | Basic if logic; runs the function if the specified choice is made in the specified decision. |
| MakeDecision ( _decision, _choice1, _choice2, [_choice3] ) | Creates a new decision and makes the player make a choice                 |
| MakeDecision ( _decision )                           | Finds the decision from the decisions array gives player the decision           |
| RunLuaScript ( _filename )                           | Runs specified Lua script from "./scripts/lua"                                  |
| Goto_debug ( _linenumber )                           | Goes immediately the specified line (for debugging)                             |
| LoadScriptGoto ( _filepath, _linenumber)             | Combination of LoadScript and Goto_debug                                        |