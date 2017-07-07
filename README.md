# HistoPlotting

## Setup commands
```Shell
git clone git@github.com:{USERNAME}/HistoPlotting.git 
cd HistoPlotting
make
```
## Enviornment
Create the following `rootLogon.C` file.  Here `{HISTOPLOTTING_DL_PATH}` is path that you downloaded the repo. For example,  `/Users/nmccoll/Dropbox/Work/GitRepositories`. Is where I put mine.

```C++
{
    gSystem->Load("{HISTOPLOTTING_DL_PATH}/HistoPlotting/libHistoPlotting.so");
    gInterpreter->AddIncludePath("{HISTOPLOTTING_DL_PATH}");
    gROOT->LoadMacro("{HISTOPLOTTING_DL_PATH}/HistoPlotting/style/CMS_lumi.C");
    gROOT->LoadMacro("{HISTOPLOTTING_DL_PATH}/HistoPlotting/style/tdrstyle.C");
    gROOT->Macro("{HISTOPLOTTING_DL_PATH}/HistoPlotting/style/setStyle.C");
}
```  

You will also need a `.rootrc` file telling root to load this `rootLogon.C`.  For example, you can place this in your working directory:  

```Shell
Rint.Logon:              rootlogon.C
Rint.Logoff:             rootlogoff.C
Unix.*.Root.MacroPath:    .:$(HOME):
```