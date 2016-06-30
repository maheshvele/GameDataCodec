FOR /L %%I IN (5,1,16) DO (
DEL OutputFiles\bitStream_%%IBits.dat 
DEL DebugFiles\compDebug_%%IBits.txt
DEL DebugFiles\decompDebug_%%IBits.txt
)

DEL RMSError.txt
DEL RMSPlot.csv