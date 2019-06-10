//$("#master").
var ipAdd;
var enCon = 1;
var flightSocket;

var throtle = "0000";
var servo = ["090", "090", "090", "090", "090"];
var flight = "m:0000s1:000s2:000s3:000s4:000r:0";
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
		$(".am").html(event.data);
	}
}

function fly() {
	flight = "m:"+throtle+"s0:"+servo[0]+"s1:"+servo[1]+"s2:"+servo[2]+"s3:"+servo[3]+"s4:"+servo[4]+"r:"+restart;
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
	$(".am").html("");
}

/**************** SLIDERS **********************************/
$("#master").on("input", function () {
	var s = $(this).val();
	var thrPerc = Math.round((s/4080)*100);
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
	if (s<10) {
		s = "00" + s;
	} else if (s<100) {
		s = "0" + s;
	}
	servo[i] = s;
	$(".serPerc"+i).html(servoAng);
});

/***************** BUTTONS **********************/
$(".restartMotorsBtn").click(function(){
	restart=1;
	$(".master").val(408);
	$(".thrVal").html(Math.round((408/4080)*100) + " %");
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
$(".colapseServoBtn").click(function () {
	var d = $(this);
	if (d.attr("data-u") == "up") {
		d.attr("data-u","down");
		d.html('<span class="fa fa-angle-double-down"></span> Show servo controls <span class="fa fa-angle-double-down"></span>');
		$(".servoColapse").slideUp(200);
	} else{
		d.attr("data-u","up");
		d.html('<span class="fa fa-angle-double-up"></span>');
		$(".servoColapse").slideDown(200);
	}
})
$(".colapseCommandBtn").click(function () {
	var d = $(this);
	if (d.attr("data-u") == "up") {
		d.attr("data-u","down");
		d.html('<span class="fa fa-angle-double-down"></span> Show flight commands <span class="fa fa-angle-double-down"></span>');
		$(".commandColapse").slideUp(200);
	} else{
		d.attr("data-u","up");
		d.html('<span class="fa fa-angle-double-up"></span>');
		$(".commandColapse").slideDown(200);
	}
})

/***********************************************/

/*************** MAIN ***************************/
setupSocket();
setInterval(function() {
	fly();
}, 100);
