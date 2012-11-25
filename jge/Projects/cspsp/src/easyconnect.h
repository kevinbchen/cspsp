/* 
 * File:   easyconnect.h
 * Author: joris
 *
 * Created on 15 augustus 2009, 19:58
 *
 */


/* EasyConnect is a class which enables you to use the PSP NetConf Dialog to
 * connect to an access point.
 *
 *
 * If you already set up the GU the minimal code would be:
 *
 *     #include "easyconnect.h"
 *     ...
 *     ...
 *     ...
 *     EasyConnect connection;
 *     connection.Connect();
 *
 * If you didn't setup the GU or uses a incompattible configuration
 * minimal code would be:
 *
 *     #include "easyconnect.h"
 *     ...
 *     ...
 *     ...
 *     EasyConnect connection;
 *     connection.InitGU();
 *     connection.Connect();
 *
 * If you want to use a onStateChange function you should inherit the
 * EasyConnect class which is explaned below.
 */

#ifndef _EASYCONNECT_H
#define	_EASYCONNECT_H

#include <string.h>//memset();

#include <pspkernel.h>
#include <psputility.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>

#include <pspgu.h>
#include <pspdisplay.h>

class EasyConnect {
public:
    EasyConnect();
    virtual ~EasyConnect();

    /* onStateChange();
     * If you want to use this function you should inherrit this class.
     *
     * class NewEasyConnect : public EasyConnect {
     * public:
     *     int onStateChange(int state){
     *         cout<<"State changed in: "<<state;
     *         return 0;
     *     }
     * }
     *
     * states:
     * 0-Disconnected
     * 1-Scanning
     * 2-Joining
     * 3-Getting IP
     * 4-Got IP (connected)
     * 5-EAP authentication
     * 6-Key exchange
     */
    virtual int onStateChange(int state);

    /* Run this to actually connect
     *
     * It returns 1 if the connection is established otherwise 0.
     */
    int Connect();

    /* You can use this function to check is the connection still exist
     *
     * It returns 1 if there is a valid connection, otherwise 0.
     */
    int IsConnected();
    
    /* Disconnect from an access point.
     */
    void Disconnect();

    /* Optionally sets the background color. (DEFAULT: 0x00000000 - BLACK)
     */
    void SetBGColor(int color);

    /* Inits the GU with the recommanded settings. Should be usable for most
     * applications. (compatible with intrafont)
     */
    void InitGU();

    /* Terminates the GU.
     */
    void TermGU();

private:
    pspUtilityNetconfData data;
    struct pspUtilityNetconfAdhoc adhocparam;
    int bgcolor;
};

#endif	/* _EASYCONNECT_H */

