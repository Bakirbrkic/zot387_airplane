//$("#master").

var throtle = "0000";
var servo = ["090", "090", "090", "090", "090"];
var flight = "m:0000s1:000s2:000s3:000s4:000r:0";
var lastFlight;
var ipAdd;
var restart=0;


ipAdd = localStorage.getItem("ipAdd");
$(".ipSpan").html(ipAdd);

var flightSocket = new WebSocket("ws://" + ipAdd, "arduino");

function fly() {
	flight = "m:"+throtle+"s0:"+servo[0]+"s1:"+servo[1]+"s2:"+servo[2]+"s3:"+servo[3]+"s4:"+servo[4]+"r:"+restart;
	restart=0;
	if (flightSocket.readyState === flightSocket.CLOSING || flightSocket.readyState === flightSocket.CLOSED) {
		$(".data").html("Connection lost, reconnecting...");
		$(".ipSpan").css("background-color", "red");
		flightSocket = null;
		flightSocket = new WebSocket("ws://" + ipAdd, "arduino");
	} else if (lastFlight != flight && flightSocket.readyState === flightSocket.OPEN) {
		flightSocket.send(flight);
		lastFlight = flight;
		$(".data").html("Connected");
		$(".ipSpan").css("background-color", "green");
	} else if (flightSocket.readyState === flightSocket.CONNECTING) {
		$(".data").html("Connecting...");
		$(".ipSpan").css("background-color", "yellow");
	}
}


$("#master").on("input", function () {
	var s = $(this).val();
	if (s<10) {
		s = "000" + s;
	} else if (s<100) {
		s = "00" + s;
	} else if (s<1000) {
		s = "0" + s;
	}
	throtle = s;
});

$(".servo, .servoH").on("input", function () {
	var i = $(this).attr("data-mnum");
	var id = $(this).attr("id");
	var s = $("#"+id).val();
	if (s<10) {
		s = "00" + s;
	} else if (s<100) {
		s = "0" + s;
	}
	servo[i] = s;
});


setInterval(function() {
	fly();
}, 100);

flightSocket.onerror = function (event) {
	//flightSocket.close();
	flightSocket = new WebSocket("ws://" + ipAdd, "arduino");
}

flightSocket.onmessage = function (event) {
	$(".data").html(event.data);
	//lastFlight = event.data;
}

$(".reconnectBtn").click(function(){
	restart=1;
})