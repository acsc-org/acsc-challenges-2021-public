<?php
    include "config.php";

    $msg  = $_POST['msg'] ?? "";
    $name = $_POST['name'] ?? "";
    if ( strlen($name) < 4 || strlen($msg) < 8 )
        highlight_file(__FILE__) && exit();

    $data = array(
        "name" => $name, 
        "msg"  => $msg, 
        "flag" => "ACSC{" . $KEY . "}" // try to get this flag!
    );

    $iv   = openssl_random_pseudo_bytes(16);
    $data = gzcompress(json_encode($data));
    $data = openssl_encrypt($data, 'aes-256-ctr', $KEY, OPENSSL_RAW_DATA, $iv);
    $data = bin2hex( $iv . $data );

    $conn = new mysqli($HOST, $USER, $PASS, $NAME);
    $sql  = sprintf("insert into msgs (msg, name) values('%s', '%s')", $data, $name);
    if (!$conn->query($sql))
        die($conn->error);
    
    echo $conn->insert_id;
    $conn->close();