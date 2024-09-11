const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">

<head>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<meta charset="UTF-8">
	<title>Tsunami Wave Data Page</title>

	<style>
		body {
			transition: background-color 0.5s, color 0.5s;
		}
		
		table {
		  position: relative;
		  width:100%;
		  border-spacing: 0px;
		}
		
		tr {
		  border: 1px solid white;
		  font-family: "Verdana", "Arial", sans-serif;
		  font-size: 20px;
		}
		
		th {
		  height: 20px;
		  padding: 3px 15px;
		  background-color: #343a40;
		  color: #FFFFFF !important;
		}
		
		td {
		  height: 20px;
		   padding: 3px 15px;
		}
		
		.tabledata {
		  font-size: 24px;
		  position: relative;
		  padding-left: 5px;
		  padding-top: 5px;
		  height:   25px;
		  border-radius: 5px;
		  line-height: 20px;
		  background-color: black;
		}
		
		.bodytext {
		  font-family: "Verdana", "Arial", sans-serif;
		  font-size: 24px;
		  text-align: left;
		  font-weight: light;
		  border-radius: 5px;
		  display:inline;
		}
		
		.navbar {
		  width: 100%;
		  height: 50px;
		  margin: 0;
		  padding: 10px 0px;
		  background-color: #87CEEB;
		  color: #000000;
		  border-bottom: 5px solid #293578;
		}
		
		.fixed-top {
		  position: fixed;
		  top: 0;
		  right: 0;
		  left: 0;
		  z-index: 1030;
		}
		
		.navtitle {
		  float: left;
		  height: 50px;
		  font-family: "Verdana", "Arial", sans-serif;
		  font-size: 50px;
		  font-weight: bold;
		  line-height: 50px;
		  padding-left: 20px;
		}
		
		.navheading {
		 position: fixed;
		 left: 60%;
		 height: 50px;
		 font-family: "Verdana", "Arial", sans-serif;
		 font-size: 20px;
		 font-weight: bold;
		 line-height: 20px;
		 padding-right: 20px;
		}
		
		.navdata {
		  justify-content: flex-end;
		  position: fixed;
		  left: 70%;
		  height: 50px;
		  font-family: "Verdana", "Arial", sans-serif;
		  font-size: 20px;
		  font-weight: bold;
		  line-height: 20px;
		  padding-right: 20px;
		}
		
		.category {
		  font-family: "Verdana", "Arial", sans-serif;
		  font-weight: bold;
		  font-size: 32px;
		  line-height: 50px;
		  padding: 20px 10px 0px 10px;
		  color: white;
		}
		
		.heading {
		  font-family: "Verdana", "Arial", sans-serif;
		  font-weight: normal;
		  font-size: 28px;
		  text-align: left;
		}
	  
		.foot {
		  font-family: "Verdana", "Arial", sans-serif;
		  font-size: 20px;
		  position: relative;
		  height:   30px;
		  text-align: center;   
		  color: #AAAAAA;
		  line-height: 20px;
		}
		
		.container {
		  max-width: 1800px;
		  margin: 0 auto;
		}
		
		table tr:first-child th:first-child {
		  border-top-left-radius: 5px;
		}
		
		table tr:first-child th:last-child {
		  border-top-right-radius: 5px;
		}
		
		table tr:last-child td:first-child {
		  border-bottom-left-radius: 5px;
		}
		
		table tr:last-child td:last-child {
		  border-bottom-right-radius: 5px;
		}
		
		button {
            background-color: black;
            border: none;
            color: white; 
            padding: 10px 10px;
            text-align: center;
            text-decoration: none;
            display: inline-block; 
            font-size: 18px;
            margin: 1px 1px; 
            cursor: pointer; 
            border-radius: 10px;
        }
		
		.colum {
			background-color: #5a5a5a;
			color: white;
			transition: background-color 0.5s, color 0.5s;
		}
	</style>
</head>
<body style="background-color: #222222;" onload="process()">
  
    <header>
		<div class="navbar fixed-top">
			<div class="container">
				<div class="navtitle" style="display: flex;">Traffic Control</p></div>
				<div class="navdata" id = "date">mm/dd/yyyy</div>
				<div class="navheading">DATE</div><br>
				<div class="navdata" id = "time">00:00:00</div>
				<div class="navheading">TIME</div>
			</div>
		</div>
    </header>
  
    <main class="container" style="margin-top:70px">
		<div class="category">Data</div>
		<div style="border-radius: 10px !important;">
			<table style="width:50%">
				<colgroup>
					<col span="1" style="background-color:rgb(230,230,230); width: 20%; color:#000000 ;">
					<col span="1" style="background-color:rgb(200,200,200); width: 15%; color:#000000 ;">
					<col span="1" style="background-color:rgb(180,180,180); width: 15%; color:#000000 ;">
				</colgroup>
				<col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
				<col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
				<col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
				<tr>
					<th colspan="1"><div class="heading">Locations</div></th>
					<th colspan="1"><div class="heading">Status</div></th>
					<th></th>
					<th></th>
					<th></th>
					<th></th>
				</tr>
				<tr>
					<td class="colum"><div class="bodytext">3rd Floor</div></td>
					<td class="colum"><div class="tabledata" id = "3fu">Up</div></td>
					<td class="colum"><div class="tabledata" id = "3fd">Down</div></td>
					<td class="colum"><button onclick="Command_Light(3, 11)">Up</button></td>
					<td class="colum"><button onclick="Command_Light(3, 01)">Down</button></td>
					<td class="colum"><button onclick="Command_Light(3, 00)">Off</button></td>
				</tr>
				<tr>
					<td class="colum"><div class="bodytext">2nd Floor</div></td>
					<td class="colum"><div class="tabledata" id = "2fu">Up</div></td>
					<td class="colum"><div class="tabledata" id = "2fd">Down</div></td>
					<td class="colum"><button onclick="Command_Light(2, 11)">Up</button></td>
					<td class="colum"><button onclick="Command_Light(2, 01)">Down</button></td>
					<td class="colum"><button onclick="Command_Light(2, 00)">Off</button></td>
				</tr>
				<tr>
					<td class="colum"><div class="bodytext">1st Floor</div></td>
					<td class="colum"><div class="tabledata" id = "1fu">Up</div></td>
					<td class="colum"><div class="tabledata" id = "1fd">Down</div></td>
					<td class="colum"><button onclick="Command_Light(1, 11)">Up</button></td>
					<td class="colum"><button onclick="Command_Light(1, 01)">Down</button></td>
					<td class="colum"><button onclick="Command_Light(1, 00)">Off</button></td>
				</tr>
				<tr>
					<td class="colum"><div class="bodytext">Timer</div></td>
					<td class="colum"><div class="tabledata" id = "Tfu">Up</div></td>
					<td class="colum"><div class="tabledata" id = "Tfd">Down</div></td>
					<td class="colum"><button onclick="Command_Time(01)">Start</button></td>
					<td class="colum"><button onclick="Command_Time(00)">Stop</button></td>
					<td class="colum"><div class="tabledata" id = "t"></div></td>
				</tr>
			</table>
		</div>
	</main>
	
	<p></p>
    
	<footer div class="foot" id = "temp" >
		<button onclick="Credits()">End Credits</button>
		</div>
	</footer>
</body>


<script type = "text/javascript">
x = 0;
document.getElementById("t").innerHTML = x;

	var d1 = "Up";
	var d2 = "Down";
	var d3 = "Up";
	var T = "Down";
	
	status_update("1", d1);
	status_update("2", d2);
	status_update("3", d3);
	status_update("T", T);



	setInterval(() => {counter();}, 1000);

    // global variable visible to all java functions
    var xmlHttp=createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject(){
		if(window.XMLHttpRequest){
			xmlHttp=new XMLHttpRequest();
		}
		
		else {
			xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
		}
		
		return xmlHttp;
    }
	
	//command the light on the their updated status
	function Command_Light(ID, Data) {
    // Format Data as a two-digit string
    var formattedData = Data.toString().padStart(2, '0');
    
    var xhttp = new XMLHttpRequest();
    
    if (ID == 1) {
        xhttp.open("PUT", "Command1?VALUE=" + formattedData, true);
    } else if (ID == 2) {
        xhttp.open("PUT", "Command2?VALUE=" + formattedData, true);
    } else if (ID == 3) {
        xhttp.open("PUT", "Command3?VALUE=" + formattedData, true);
    }
    
    xhttp.send();
	}

	
	function Command_Time(Data) {
		var formattedData = Data.toString().padStart(2, '0');

		var xhttp = new XMLHttpRequest();
		xhttp.open("PUT", "Timer?VALUE="+formattedData, true);
		xhttp.send();
	}
	
	function Credits() {
		alert("Credits to the following people on the Team \n \n-Matthew Misa \n \n 'This was really fun to work with and we hope that people will solve problems'");
	}
	
	function status_update(ID, Data) {

		let Up = String(ID) + "fu";
		let Down = String(ID) + "fd";

		if (Data == "Up"){
			document.getElementById(Up).style.backgroundColor = "green";
			document.getElementById(Down).style.backgroundColor = "red";
		}
		if (Data == "Down"){
			document.getElementById(Up).style.backgroundColor = "red";
			document.getElementById(Down).style.backgroundColor = "green";
		}
		if (Data == "None"){
			document.getElementById(Up).style.backgroundColor = "gray";
			document.getElementById(Down).style.backgroundColor = "gray";
		}
	}
	
	function counter() {
		x = x+1;
		document.getElementById("t").innerHTML = x;
	}
	
    // function to handle the response from the ESP XML BOI
    function response(){
		var message;
		var currentsensor;
		var xmlResponse;
		var xmldoc;
		var dt = new Date();

		// get the xml stream
		xmlResponse=xmlHttp.responseXML;
  
		// get host date and time
		document.getElementById("time").innerHTML = dt.toLocaleTimeString();
		document.getElementById("date").innerHTML = dt.toLocaleDateString();
  
		// 1st floor status
		xmldoc = xmlResponse.getElementsByTagName("f1");
		let message1 = xmldoc[0].firstChild.nodeValue;
		status_update("1", message1);

		// 2nd Floor
		xmldoc = xmlResponse.getElementsByTagName("f2");
		let message2 = xmldoc[0].firstChild.nodeValue;
		status_update("2", message2);
		
		// 3rd Floor
		xmldoc = xmlResponse.getElementsByTagName("f3");
		let message3 = xmldoc[0].firstChild.nodeValue;
		status_update("3", message3);
		
		//timer 
		xmldoc = xmlResponse.getElementsByTagName("fT");
		let message4 = xmldoc[0].firstChild.nodeValue;
		status_update("T", message4);
    }
  
  
    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to 
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process(){
     
		if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
			xmlHttp.open("PUT","xml",true);
			xmlHttp.onreadystatechange=response;
			xmlHttp.send(null);
		}	       
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("process()",50);
    }
	
	/*
	 get it? here's the xml message example:
	<?xml version="1.0" encoding="UTF-8"?>
		<data>
			<1>Down</1>
			<2>Up</2>
			<3>Up</2>
		</data>
	*/
  
  
</script>

</html>



)=====";