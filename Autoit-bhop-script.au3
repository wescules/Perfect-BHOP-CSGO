#include <Misc.au3>
; 0 to stop the program
HotKeySet("0", "_Exit")
; focuses on csgo
WinActivate("Counter-Strike: Global Offensive")
Local $hDLL = DllOpen("user32.dll")

	While 1
	; if space is pressed its spams it until space is released
	If _IsPressed("20", $hDLL) Then
		Send("{SPACE}")
		Send("{SPACE}")
		Send("{SPACE}")
		Send("{SPACE}")
		IF _IsPressed("41", $hDLL) Or _IsPressed("44", $hDLL) Then
			Send("{SPACE}")
			Send("{SPACE}")
			Send("{SPACE}")
			Send("{SPACE}")
		EndIf
	EndIf
	WEnd

DllClose($hDLL)
Func _Exit()
	Exit
	EndFunc
