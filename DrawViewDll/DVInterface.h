#ifndef DVINTERFACE
#define DVINTERFACE

#ifdef DV_EXPORTS
#define DV_INTF __declspec(dllexport)
#else
#define DV_INTF __declspec(dllimport)
#endif

extern "C" DV_INTF void run();
extern "C" DV_INTF void stop();
extern "C" DV_INTF void set_data_size(int s);
extern "C" DV_INTF void PushData(char * p);

#endif