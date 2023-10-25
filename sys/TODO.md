# System TODO

## Yellowstone

- Add registration to the base yellowstone yunq service
  - Requires: Adding ability to pass capabilities in endpoint calls.
- Store registered services in a hashmap.
  - Requires: Adding hashmap to Glacier.
- Start the next service from a configuration file.

## Denali

- Migrate denali to a yunq definition.
  - Requires: Adding async yunq servers (or more synchronization primitives)
- Add the ability to send multiple Ahci commands at once.

## VictoriaFalls

- Add a read file VFS service.
