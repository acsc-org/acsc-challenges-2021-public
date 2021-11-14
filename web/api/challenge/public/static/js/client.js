function signin(){
	var xhr = new XMLHttpRequest();
	xhr.open("POST", "/api.php", false);
	xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	xhr.send("id=" + document.getElementsByClassName('form-control')[0].value + "&pw=" + document.getElementsByClassName('form-control')[1].value + "&c=i");
	if (xhr.responseText)
		document.getElementById("result").innerHTML = xhr.responseText;
	else
		alert("Only admin can access the page");
}
function signup(){
	var xhr = new XMLHttpRequest();
	xhr.open("POST", "/api.php", false);
	xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	xhr.send("id=" + document.getElementsByClassName('form-control')[0].value + "&pw=" + document.getElementsByClassName('form-control')[1].value + "&c=u");
	document.getElementById("result").innerHTML = xhr.responseText;
}
