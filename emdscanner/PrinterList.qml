import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
//import com.client.emindprint 1.0
import QtQuick.Layouts 1.1
import "client.js" as Jsclient
import QtQuick.Dialogs 1.2

Item {
    objectName: "PrinterWin"
    id:printerlist
    height: 500
    width: 600
    visible:false
    property string pnameStr1: ""
    property var pNameList:[]
    property var m_added
    signal showList()
    signal stopSpinner()

    WorkerScript{
        id:worker
        source:"dataloader.js"
    }

    WorkerScript{
        id:prnWorker
        source:"addprinters.js"
    }


    function cmpPname(prnName)
    {
        console.log("cmpPname");
        var tmpName = prnName.replace(/_/g," ");
        tmpName = tmpName.replace(/\*/g,"/");
        console.log("tmpName=",tmpName);
        var pname = new Array;
        pname= Jsclient.g_str.split(',');
//        tmpName = tmpName.split(',');
        console.log("pModel.count=",pModel.count);
        for(var i = 0;i < pname.length;i++){
            if(tmpName == pname[i]){
                console.log("pname[%1]=".arg(i),pname[i]);
               worker.sendMessage({"index":i,"prstate":qsTr("Added"),"model":pModel});

            }
        }
    }

    function delPname(prnName)
    {
        console.log("delPname");
        var tmpName = prnName.replace(/_/g," ");
        tmpName = tmpName.replace(/\*/g,"/");
        console.log("tmpName=",tmpName);
        var pname = new Array;
        pname= Jsclient.g_str.split(',');
//        tmpName = tmpName.split(',');
        console.log("pModel.count=",pModel.count);
        for(var i = 0;i < pname.length;i++){
            if(tmpName == pname[i]){
                console.log("pname[%1]=".arg(i),pname[i]);
               worker.sendMessage({"index":i,"prstate":qsTr("Add"),"model":pModel});

            }
        }
    }

    onShowList:{
        var pname = new Array;
        pname= Jsclient.g_str.split(',');
        console.log(pname);
//        for(var i=0;i<pname.length;i++){
//            pModel.append({"prname":pname[i],"prstate": qsTr(Jsclient.pstate)});
//        }
        prnWorker.sendMessage({"prname":pname,"model":pModel,"buttonText":qsTr("Add")});

        clienter.searchPPD();
    }




    ListModel{
        id:pModel
    }

    ListView{
        id:pView
        anchors.fill:parent
        model:pModel
        delegate:pDelegate
        anchors.margins: 15
        Layout.alignment: Qt.AlignCenter
    }


    Component{
        id:highlighter
        Rectangle{
            z:1
            opacity: 0.8
            width: pView.width
            height:60
            radius: 5
            color:"#f5f5f5"
        }
    }




    Component{
        id:pDelegate

        Rectangle{
            id:printerItem
            width:parent.width
            height:60

            Image{
                id:pImg
                height: 40
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                source:"img/printer.png"
            }

            Text{
                id:printerName
                text:prname
                font.pixelSize: 18
                anchors.left: parent.left
                anchors.leftMargin: pImg.width
                anchors.verticalCenter: parent.verticalCenter
            }


            Component{
                id:btnStyle3
                ButtonStyle{
                    background: Rectangle{
                        width:control.width
                        height:control.height
                        color:printerItem.color
                    }
                    label:Text{
                        text:prstate
                        font.pixelSize: 18
                        anchors.fill: parent
//                        color:control.hovered?"#0087ff":"black"
                    }
                }
            }


            MouseArea{
                id:itemMouseArea
                hoverEnabled: true
                anchors.fill: parent
                onHoveredChanged: {
                    pView.currentIndex = index;
                }
                onEntered: {
                    printerItem.color = "#f5f5f5";
                }
                onExited: {
                    printerItem.color = "white";
                }
            }

            Button{
                id:btnAdd
                width:60
                height: 40
                anchors.right: printerItem.right
                anchors.verticalCenter: parent.verticalCenter
                style:ButtonStyle{
                    id:btnAddStyle
                    background: Rectangle{
                        width:control.width
                        height:control.height
                        color: printerItem.color
                    }
                    label:Text{
                        id:btnText
                        color:control.hovered?"#0087ff":"black"
                        //text:busyIndicator.running ? qsTr("") : qsTr("Add")
                        //text:control.pressed?qsTr("Added"):qsTr("Add")
                        text:prstate
                        font.pixelSize: 18
                        anchors.fill: parent
                        //wrapMode: Text.WordWrap
                    }
                }

                onClicked: {
                    console.log("printerName.text=",printerName.text);
                    console.log("index=",index);
                    console.log("Jsclient.g_ip=",Jsclient.g_ip);
                    clienter.sigSetDefPrinter(Jsclient.g_ip,printerName.text,index);
                    worker.sendMessage({"index":index,"prstate":qsTr("Added"),"model":pModel});
                    btnAdd.style = btnStyle3;
                }

            }

        }
    }


}



