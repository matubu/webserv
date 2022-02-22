<?php
$uploaddir = 'upload/';

if (isset($_FILES['file']))
{
	$uploadfile = $uploaddir . basename($_FILES['file']['name']);

	if (move_uploaded_file($_FILES['file']['tmp_name'], $uploadfile))
		echo "file successfully uploaded";
	else
		echo "file couldn't be uploaded";
}