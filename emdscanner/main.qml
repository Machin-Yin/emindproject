import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import com.client.emindprint 1.0
import "client.js" as Jsclient
import QtQuick.Dialogs 1.2

ApplicationWindow {
    objectName: "rootObj"
    id:root
    height: 240
    maximumHeight: 240
    width: 400
    maximumWidth: 400
    visible:true
    flags:Qt.WindowMinimizeButtonHint|Qt.WindowCloseButtonHint
    title: qsTr("EmindCloudScanner")
    style: ApplicationWindowStyle {
        background:Rectangle{
            color:"white"
        }
    }

    Clienter{
        id:clienter
        signal showList()
        signal prnAdded(var addedPname)
        signal prnDel(var delPname)
        onSigConnected: {
            console.log("sig connected");
            stackView.push({item:pwin});
            root.width = 600;
            root.maximumWidth=600;
            root.maximumHeight=500;
            root.height = 500;
            root.title = qsTr("Add Printer");
        }
        onSigPlistSent:{
            Jsclient.g_str = clienter.pnameStr;
            //            console.log("ClientLogin.qml:Jsclient.g_str=",Jsclient.g_str);
            showList();
        }
        onSigRunningWarn:{
            msgDialog.title = qsTr("EmindCloudScanner");
            msgDialog.text = qsTr("EmindCloudScanner Is Running!");
            msgDialog.visible = true;
        }
        onPrinterAdded:{
            //            prnAdded(clientAssist.pnameStr);
            console.log("clienter.pnameAdded=",clienter.pnameAdded);
            prnAdded(clienter.pnameAdded);
        }
        onSigOffline:{
            stackView.enabled=false;
            layered.visible = true;
        }
        onSigPrnDeleted:{
            console.log("onSigPrnDeleted");
            console.log("clienter.pnameDel=",clienter.pnameDel);
            prnDel(clienter.pnameDel);
        }

    }


    Component{
        id:pwin
        PrinterList{
            id:printerwin
            visible: true
            Connections{
                target: clienter
                onShowList:{
                    printerwin.showList();
                }
            }
            Connections{
                target: clienter
                onPrnAdded:{
                    console.log("onPrnAdded=",addedPname);
                    printerwin.cmpPname(addedPname);
                }
            }

            Connections{
                target: clienter
                onPrnDel:{
                    console.log("onPrnDel=",delPname);
                    printerwin.delPname(delPname);
                }
            }
        }
    }




    Item {
        id: layered
        opacity: 0.3
        visible: false

        implicitHeight : root.height
        implicitWidth : root.width
        Rectangle { width: root.width; height: root.height; color: "grey"; }
        z:8888
    }


    MessageDialog{
        id:msgDialog
    }



    Component{
        id:cwin
        ClientLogin{
            id:clientwin
            visible: true
        }
    }

    StackView{
        id:stackView
        anchors.fill: parent
        initialItem: {
            item:cwin
        }
    }



}
