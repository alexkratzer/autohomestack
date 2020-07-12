# database structure

## master_data_plc_actor


Name|Type|Comments
-|-|-
id|int(11)|not used jet...
plc|varchar(20)|name of plc actor belongs to
actor_type|enum('light', 'jalousie', 'heater', '')|enum of actor types
actor_name|varchar(40)|name of actor, defined in plc
property_key|varchar(20)|name of property
property_type|varchar(20)|datatype of the value (bool, integer, ...)
property_value_startup|varchar(20)|value for startup config
property_value_current|varchar(20)|not used jet...

udp interface
-|-|-

logic interface
-|-|-

