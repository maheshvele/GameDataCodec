FOR /L %%I IN (5,1,16) DO (
DEL OutputFiles\bitStream_%%IBits.dat 
DEL DebugFiles\decompDebug_%%IBits.dat
DEL DebugFiles\compDebug_%%IBits.dat
)

DEL RMSError.txt
DEL RMSPlot.csv

FOR /L %%I IN (5,1,16) DO (
Debug\Compressor.exe %%I
COPY bitStream.dat OutputFiles\bitStream_%%IBits.dat 
COPY compDebug.txt DebugFiles\compDebug_%%IBits.txt
Debug\Decompressor.exe >> RMSError.txt
COPY decompDebug.txt DebugFiles\decompDebug_%%IBits.txt

DEL compDebug.txt
DEL decompDebug.txt
DEL bitStream.dat

)

COPY RMSError.txt RMSPlot.csv