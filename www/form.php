<html>
<body>

<form method="post" action="/form.php">
  <input type="text" name="Name">
  <input type="submit">
</form>

<?php

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  // collect value of input field
  echo "I got the content trust me bro";
  $entityBody = file_get_contents('php://input');
  echo $entityBody;

}
?>

</body>
</html> 