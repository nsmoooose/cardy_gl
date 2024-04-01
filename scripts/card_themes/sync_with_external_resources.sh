#!/bin/bash
set -e

destination=$(pwd)/../../resources/card_decks/french/themes
if [[ ! -d $destination ]]; then
    echo "ERROR: '$destination' does not exist!"
    exit 1
fi

while IFS=$'\t\r' read -r name directory license_url cards_url corner_width
do
    echo "$name @ $cards_url"
    mkdir -p "$destination/$directory"
    curl -s "$cards_url" > "$destination/$directory/cards.svg"
    curl -s "$license_url" > "$destination/$directory/README"
    echo "$name" > "$destination/$directory/name"
    echo "$corner_width" > "$destination/$directory/corner_width"
done < <(jq ".[] | [.name, .dir, .license_url, .cards_url, .corner_width] | @tsv" ./external_card_resources.json -r)

for filename in $(find "$destination" -name "*.svg"); do
    sed -i -f fix_cards.sed "$filename"
done
