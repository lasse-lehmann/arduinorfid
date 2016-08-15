# arduinorfid
Arduino Project that verifies that specific RFID chips are placed on 3 readers connected to an Arduino in a specific order.
The readers are placed under the lid of a closed box.
The project implements a riddle involving the correct card to be placed on each reader in order to open the box.
For each reader only one card is correct. In order to solve the riddle cards must be placed on the lid of the box in the correct places. If a card is placed incorrectly the riddle gets reset. In order to re-read cards that have been placed already, the code continously re-ready cards on all the readers even if they did not change.

I use https://github.com/miguelbalboa/rfid for all the hard work reading the tags and interacting with the readers. Great work there, thanks.

