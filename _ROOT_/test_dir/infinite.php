#!/usr/bin/env php
<?php
// Infinite loop printing a message every second

while (true) {
    echo "Hello from PHP!\n";
    sleep(1);  // wait 1 second to avoid flooding the terminal
}
?>
