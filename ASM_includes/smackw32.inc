IFNDEF SMACKW32_INC
SMACKW32_INC equ <1>

extrn _SmackClose:proc
extrn _SmackColorTrans:proc
extrn _SmackDoFrame:proc
extrn _SmackNextFrame:proc
extrn _SmackOpen:proc
extrn _SmackSoundUseDirectSound:proc
extrn _SmackToBuffer:proc
extrn _SmackWait:proc

ELSE
  echo -------------------------------------------
  echo WARNING duplicate include file smackw32.inc
  echo -------------------------------------------
ENDIF
