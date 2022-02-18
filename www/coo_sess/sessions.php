<?php
session_start();

if (isset($_SESSION["test"]))
	echo "Ok Session<br>";

if (isset($_COOKIE["test"]))
	echo "Ok Cookie<br>";

