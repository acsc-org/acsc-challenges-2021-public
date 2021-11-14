<?php
    include "/var/www/html/config.php";
    $conn = new mysqli($HOST, $USER, $PASS, $NAME);
    $conn->query("DELETE FROM msgs");
    $conn->close();