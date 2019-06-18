//$("#master").
var ipAdd;
var enCon = 1;
var flightSocket;

var throtle = "000";
var servo = ["090", "090", "090", "090", "090"];
var flight = "m:000w:000t:000e:000r:0";
var lastFlight;
var restart=0;


ipAdd = localStorage.getItem("ipAdd");
$(".ipSpan").html(ipAdd);


function setupSocket() {
	flightSocket = new WebSocket("ws://" + ipAdd, "arduino");
	
	flightSocket.onerror = function (event) {
		//flightSocket.close();
		flightSocket = null;
		flightSocket = new WebSocket("ws://" + ipAdd, "arduino");
	}

	flightSocket.onmessage = function (event) {
		var data = ""+event.data;
		$(".am").html(data);
		//$(".batLvlVal").html(data.substring(data.indexOf("battery: ")));
		//$(".downVal").html(data.substring(data.indexOf("d: "), data.indexOf("battery: ")));
		$(".gyroTiltVal").html(data.substring(data.indexOf("Angle X: ")+9, data.indexOf("Angle Y: ")));
		$(".gyroElevVal").html(data.substring(data.indexOf("Angle Y: ")+9));
		// [M] Sensors: d: 0000 battery: 14.53
	}
}

function fly() {
	flight = "m:"+throtle+"w:"+servo[0]+"e:"+servo[2]+"t:"+servo[4]+"r:"+restart;
	$(".fc").html(flight);
	restart=0;
	if (flightSocket.readyState === flightSocket.CLOSING || flightSocket.readyState === flightSocket.CLOSED) {
		$(".data").html("Connection lost");
		$(".ipSpan").css("background-color", "red");
		flightSocket = null;
		if (enCon) {
			setupSocket();
		}
	} else if (lastFlight != flight && flightSocket.readyState === flightSocket.OPEN) {
		flightSocket.send(flight);
		lastFlight = flight;
		$(".data").html("Connected");
		$(".ipSpan").css("background-color", "green");
	} else if (flightSocket.readyState === flightSocket.CONNECTING) {
		$(".data").html("Connecting...");
		$(".ipSpan").css("background-color", "yellow");
	}
	//$(".am").html("");
}

/**************** SLIDERS **********************************/
$("#master").on("input", function () {
	var s = $(this).val();
	var thrPerc = s;//Math.round((s/4080)*100);
	if (s<10) {
		s = "000" + s;
	} else if (s<100) {
		s = "00" + s;
	} else if (s<1000) {
		s = "0" + s;
	}
	throtle = s;
	$(".thrVal").html(thrPerc + " %");
});

$(".servo, .servoH").on("input", function () {
	var i = $(this).attr("data-mnum");
	var id = $(this).attr("id");
	var s = $("#"+id).val();
	var servoAng = s + "°"
	
	if (i == "0") {
		if (s<10) {
			s = "00" + s;
		} else if (s<100) {
			s = "0" + s;
		}
		servo[1] = s;
		servo[0] = s;
		$(".serPerc"+i).html(servoAng);
	}

	if (i == "2") {
		if (s<10) {
			s = "00" + s;
		} else if (s<100) {
			s = "0" + s;
		}
		var s2 = 180-s;
		if (s2<10) {
			s2 = "00" + s2;
		} else if (s2<100) {
			s2 = "0" + s2;
		}
		servo[3] = s2;
		servo[2] = s;
		$(".serPerc"+i).html(servoAng);
	}

	if (i == "4") {
		if (s<10) {
			s = "00" + s;
		} else if (s<100) {
			s = "0" + s;
		}
		servo[4] = s;
		$(".serPerc"+i).html(servoAng);
	}
});

/***************** BUTTONS **********************/
$(".restartMotorsBtn").click(function(){
	restart=1;
	$(".master").val(0);
	$(".thrVal").html(0 + " %");
})
$(".resetServosBtn").click(function(){
	for (var i = servo.length - 1; i >= 0; i--) {
		servo[i] = "090";
		$(".serPerc"+i).html(90+ "°");
		$("#servo"+i).val(90);
	}
	$(".servoH").val(90);
})

$(".sideDisconnectBtn").click(function() {
	enCon = 0;
	flightSocket.close();
	$(".disconnectBtn div, .sideDisconnectBtn").css("background-color","red");
})

$(".disconnectBtn").click(function() {
	enCon = 0;
	flightSocket.close();
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

//sminka btns
$(".colapseBtn").click(function () {
	var d = $(this);
	if (d.attr("data-u") == "up") {
		d.attr("data-u","down");
		d.html('<span class="fa fa-angle-double-down"></span> '+d.attr("data-lbl")+' <span class="fa fa-angle-double-down"></span>');
		$("."+d.attr("data-colapse")).slideUp(200);
	} else{
		d.attr("data-u","up");
		d.html('<span class="fa fa-angle-double-up"></span>');
		$("."+d.attr("data-colapse")).slideDown(200);
	}
})
/***********************************************/

/*************** MAIN ***************************/
setupSocket();
setInterval(function() {
	fly();
}, 100);

//temp bat, temp vani, pritisak, vlaznost, altitude, distance leve desne, on of state ledova, on of blic, puni ne puni