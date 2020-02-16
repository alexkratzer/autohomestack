# field level

## data logger

During the early implementing phase of the home automation system i needed some logging mechanism for bug finding.

And after implementing this interface at the plc and i kept it for data logging.

Meanwhile the usual data logging is realized within the control system (node red) but for redundancy purpose the mechanism is still alive.

The main script provides a IP listener for every configuret plc connection.

Messages from the remote clients will be parsed and stored in a db.
