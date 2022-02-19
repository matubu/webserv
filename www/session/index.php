<?php
session_start();

$_SESSION["test"] = rand();
setcookie("test", "ok", time()+3600);
?>

<a href="/session/sessions.php">view session info</a>