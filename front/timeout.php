<?php
	if ($_GET['t'] == '0') {
		file_get_contents("http://127.0.0.1:8080/login.php");
	} else {
		sleep(90000);
	}
?>
