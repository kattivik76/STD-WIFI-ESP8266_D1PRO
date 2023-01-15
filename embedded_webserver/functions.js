// JavaScript Document
function openForm() {
	document.getElementById("loginPopup").style.display="block";
}
      
function closeForm() {
	document.getElementById("loginPopup").style.display= "none";
}
// When the user clicks anywhere outside of the modal, close it

window.onclick = function(event) {
	var modal = document.getElementById('loginPopup');
	if (event.target == modal) {
		closeForm();
	}
}

var attempt = 3; // Variable to count number of attempts.
// Below function Executes on click of login button.
function validate(){
	username = document.getElementById("username").value;
	password = document.getElementById("password").value;
	if ( username == "admin" && password == "admin"){
		delete username;
		delete password;
		//alert ("Login successfully");
		window.location = "settings.html"; // Redirecting to other page.
		return false;
	}
	else{
		attempt --;// Decrementing by one.
		alert("You have left "+attempt+" attempt;");
		// Disabling fields after 3 attempts.
		if( attempt == 0){
		document.getElementById("username").disabled = true;
		document.getElementById("password").disabled = true;
		document.getElementById("submit").disabled = true;
		return false;
		}
	}
}