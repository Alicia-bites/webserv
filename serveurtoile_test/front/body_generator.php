<?php
	$size = $_GET['size'];
    srand($_GET['seed']);
    for ($i = 0; $i < $size; $i++) {
        echo chr(rand(65, 90));
    }
?>