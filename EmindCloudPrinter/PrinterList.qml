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
    //    minimumHeight: 500
    //    maximumWidth: 600
    width: 600
    visible:false

    //    title:qsTr("Add Remote Printers")
    property string pnameStr1: ""
    property var pNameList:[]
    property bool m_added: false
    signal showList()
    signal stopSpinner()
    //    signal prnAdded(variant str)
    property string addedPname
    signal addedChanged(bool m_isAdded)

    //    signal setDefaultPrinter(var text,var idx)

    //    signal printerAdded(string prName)
    //    property string printerName

    //    PrinterListModel{
    //        id:pModel
    //    }
    //    EmindClient{
    //        id:client
    //    }

    //    Connections{
    //        target: printerlist
    //        onShowList:{
    //            var pname = new Array;
    //            pname= Jsclient.g_str.split(',');
    //            console.log(pname);
    //            for(var i=0;i<pname.length;i++)
    //                pModel.append({"prname":pname[i]});
    //        }

    //    }

    function cmpPname(prnName)
    {
        console.log("cmpPname");
        var tmpName = prnName.replace(/_/g," ");
        console.log("tmpName=",tmpName);
        var pname = new Array;
        pname= Jsclient.g_str.split(',');
        for(var i = 0;i < pname.length;i++){
            console.log("pname[%1]=".arg(i),pname[i]);
            if(tmpName === pname[i]){
                pView.currentIndex = i;
                m_added = false;
                console.log("m_added=",m_added);
                //                    pView.currentItem.btnAdd.style=btnStyle4;
                //                    btnAdd.style = btnStyle4;
//                isAdded(tmpName);
                addedChanged(false);

            }
        }
    }



//    function isAdded(prnName){
//        if(prnName === prnText  ){
//            addedChanged(true);
//            return qsTr("Added");
//        }
//        else{
//            addedChanged(false);
//            return qsTr("Add");
//        }
//    }



    onShowList:{
        var pname = new Array;
        pname= Jsclient.g_str.split(',');
        console.log(pname);
        //        pModel.clear();
        for(var i=0;i<pname.length;i++)
            pModel.append({"prname":pname[i]});

    }


    Component{
        id:btnStyle4
        ButtonStyle{
            background: Rectangle{
                width:control.width
                height:control.height
                color:printerItem.color
            }
            label:Text{
                color:control.hovered?"#0087ff":"black"
                text:m_added?qsTr("Added"):qsTr("Add")
                font.pixelSize: 18
                anchors.fill: parent

            }
        }
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
        //        highlight: highlighter
        //        highlightFollowsCurrentItem: true
        //        focus:true
        Layout.alignment: Qt.AlignCenter
        //        z:-1
        onAddChanged: {
            console.log("added");
            m_added = false;
        }
    }


    Component{
        id:highlighter
        Rectangle{
            z:1
            opacity: 0.8
            width: pView.width
            height:60
            radius: 5
            //            MouseArea{
            //                hoverEnabled: true
            //                anchors.fill: parent
            //                onHoveredChanged: {
            //                    parent.color = "#f5f5f5";
            //                }
            //            }
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
                id:btnStyle1
                ButtonStyle{
                    background: Rectangle{
                        width:control.width
                        height:control.height
                        color:printerItem.color
                    }
                    label:Text{
                        color:control.hovered?"#0087ff":"black"
                        text:m_added?qsTr("Added"):qsTr("Add")
                        font.pixelSize: 18
                        anchors.fill: parent

                    }
                }
            }

            Component{
                id:btnStyle2
                ButtonStyle{
                    background: Rectangle{
                        width:control.width
                        height:control.height
                        color:printerItem.color
                    }
                    label:Text{
                        color:control.hovered?"#0087ff":"black"
                        text:m_added?qsTr("Added"):qsTr("Add")
                        font.pixelSize: 18
                        anchors.fill: parent

                    }
                }
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
                        text:qsTr("Added")
                        font.pixelSize: 18
                        anchors.fill: parent

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

            //            state:State{
            //                name:"stAdded"
            //                PropertyChanges {
            //                    target: pDelegate

            //                }
            //            }

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
                        //                        text:busyIndicator.running ? qsTr("") : qsTr("Add")
                        text:control.pressed?qsTr("Added"):qsTr("Add")
                        font.pixelSize: 18
                        anchors.fill: parent
                        //                        wrapMode: Text.WordWrap
                    }
                }

//                Text{
//                    width: 60
//                    height: 40
//                    anchors.fill:parent
//                    font.pixelSize: 18
//                    text:m_added?qsTr("Add"):qsTr("Added")

//                }

                onClicked: {
                    busyIndicator.visible = true;
                    busyIndicator.running = true;
                    //                    console.log("index=",index);
                    clientAssist.setDefaultPrinter(printerName.text,index);
                    //                    busyIndicator.visible = false;
                    //                    busyIndicator.running = false;
                    btnAdd.style = btnStyle3;
                    m_added = true;
                    //                    btnAdd.enabled = false;
                }


                Connections{
                    target:printerlist
                    onAddedChanged:{
                        console.log("onAddedChanged");
//                        m_added = m_isAdded;
//                        btnAdd.pressed();
                    }
                }

                BusyIndicator {
                    id:busyIndicator
                    anchors.fill: parent
                    width:45
                    height: 45
                    visible: false
                    running: false
                    Timer{
                        id:timer
                        interval: 3000
                        running: true
                        repeat: true
                        onTriggered:
                        {
                            busyIndicator.visible = false;
                            busyIndicator.running = false;
                        }

                    }

                    Connections{
                        target: printerlist
                        onStopSpinner:{
                            timer.start();
                        }
                    }


                }


            }
//            Binding{
//                target:printerName
//                property: 'text'
//                value:isAdded()
//                when:ListView.isCurrentItem
//            }

        }
    }


}



