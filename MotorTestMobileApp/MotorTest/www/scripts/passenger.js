//$("#master").
var ipAdd;
var enCon = 1;
var passengerSocket;

var restart=0;


ipAdd = localStorage.getItem("ipAdd");
$(".ipSpan").html(ipAdd);


function setupSocket() {
	passengerSocket = new WebSocket("ws://" + ipAdd, "arduino");
	
	passengerSocket.onerror = function (event) {
		passengerSocket.close();
		passengerSocket = null;
		passengerSocket = new WebSocket("ws://" + ipAdd, "arduino");
	}

	passengerSocket.onmessage = function (event) {
		var data = ""+event.data;
		$(".am").html(data);
		$(".gyroTiltVal").html(data.substring(data.indexOf("Angle X: ")+9, data.indexOf("Angle Y: ")));
		$(".gyroElevVal").html(data.substring(data.indexOf("Angle Y: ")+9));
		// [M] Sensors: d: 0000 battery: 14.53
	}
}

function fly() {
	if (passengerSocket.readyState === passengerSocket.CLOSING || passengerSocket.readyState === passengerSocket.CLOSED) {
		$(".data").html("Connection lost");
		$(".ipSpan").css("background-color", "red");
		passengerSocket = null;
		if (enCon) {
			setupSocket();
		}
	} else if (passengerSocket.readyState === passengerSocket.OPEN) {
		$(".data").html("Connected");
		$(".ipSpan").css("background-color", "green");
	} else if (passengerSocket.readyState === passengerSocket.CONNECTING) {
		$(".data").html("Connecting...");
		$(".ipSpan").css("background-color", "yellow");
	}
}

/***************** BUTTONS **********************/
$(".sideDisconnectBtn").click(function() {
	enCon = 0;
	passengerSocket.close();
	$(".disconnectBtn div, .sideDisconnectBtn").css("background-color","red");
})

$(".disconnectBtn").click(function() {
	enCon = 0;
	passengerSocket.close();
	$(".disconnectBtn div, .sideDisconnectBtn").css("background-color","red");
})

$(".sideReconnectBtn, .reconnectBtn").click(function() {
	enCon = 1;
	setupSocket();
	$(".sideDisconnectBtn").css("background-color","white");
	$(".disconnectBtn div").css("background-color","rgba(0,0,0,0.4)");
	$(".data").html("Connecting...");
	$(".ipSpan").css("background-color", "yellow");
})
/***********************************************/

/*************** MAIN ***************************/
setupSocket();
setInterval(function() {
	read();
}, 100);

//temp bat, temp vani, pritisak, vlaznost, altitude, distance leve desne, on of state ledova, on of blic, puni ne puni