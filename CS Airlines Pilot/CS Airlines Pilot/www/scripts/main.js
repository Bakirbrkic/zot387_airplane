var cah = $(".controlAreaLeft").height()-50;
var throtle = "0000";
var yaw, pitch, roll = 0;
var servo = ["090", "090", "090", "090", "090"];
var flight = "m:0000s1:000s2:000s3:000s4:000r:0";
var lastFlight;
var ipAdd;
var restart=0;


ipAdd = localStorage.getItem("ipAdd");
$(".infoAddress").html(ipAdd);

var flightSocket = new WebSocket("ws://" + ipAdd, "arduino");

function fly() {
	flight = "m:"+throtle+"s0:"+servo[0]+"s1:"+servo[1]+"s2:"+servo[2]+"s3:"+servo[3]+"s4:"+servo[4]+"r:"+restart;
	$(".infoCommand").html(flight);
	restart=0;
	if (flightSocket.readyState === flightSocket.CLOSING || flightSocket.readyState === flightSocket.CLOSED) {
		$(".infoConnected").html("Connection lost");
		$(".infoConnected").parent().css("background-color", "rgba(255,0,0,0.6)");
		flightSocket = null;
		flightSocket = new WebSocket("ws://" + ipAdd, "arduino");
	} else if (lastFlight != flight && flightSocket.readyState === flightSocket.OPEN) {
		flightSocket.send(flight);
		lastFlight = flight;
		$(".infoConnected").html("Connected");
		$(".infoConnected").parent().css("background-color", "rgba(0,255,0,0.6)");
	} else if (flightSocket.readyState === flightSocket.CONNECTING) {
		$(".infoConnected").html("Connecting...");
		$(".infoConnected").parent().css("background-color", "rgba(255,255,0,0.6)");
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

$(".resetMotorsSwitch").click(function(){
	restart=1;
	console.log("r: " + restart + "flight: " + flight);
})


function throtlePerc(throtle, cah) {
	return ((throtle / cah) * -100).toFixed(0);
}


$(".jLeft").draggable({
    revert: false,
    containment: "parent",
    create: function(){
        $(this).data("startLeft",parseInt($(this).parent().width()/2-25));
        $(this).data("startTop",parseInt($(this).parent().height()-50));
    },
    drag: function(event,ui){
        yaw = ui.position.left - parseInt($(this).parent().width()/2-25);
        throtle = ui.position.top - parseInt($(this).parent().height()-50);
        $(".infoThrotle").html(throtlePerc(throtle, cah));
        //$('#coordsL').text(rel_left + ", " + rel_top);
    },
    stop: function(){
    	$(this).css("top","calc(100% - 50px + "+throtle+"px)");
    	$(this).css("left","calc(50% - 25px)");
    }
});

$(".jRight").draggable({
    revert: true,
    containment: "parent",
    create: function(){
        $(this).data("startLeft",parseInt($(this).parent().width()/2-25));
        $(this).data("startTop",parseInt($(this).parent().height()/2-25));
    },
    drag: function(event,ui){
        roll = ui.position.left - parseInt($(this).parent().width()/2-25);
        pitch = ui.position.top - parseInt($(this).parent().height()/2-25);
        //$('#coordsR').text(rel_left + ", " + rel_top);
    },
    stop: function(){

    }
});