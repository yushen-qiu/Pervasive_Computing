// Definition of centralized Google Places types list
#include <config/PlaceTypes.h>

namespace Config {

// Keep this list in sync with supported types from the Places API
const char* const PLACE_TYPES[] = {"beach",
                                   "art_gallery",
                                   "cultural_landmark",
                                   "historical_place",
                                   "monument",
                                   "museum",
                                   "sculpture",
                                   "amphitheatre",
                                   "amusement_center",
                                   "amusement_park",
                                   "botanical_garden",
                                   "cultural_center",
                                   "dog_park",
                                   "ferris_wheel",
                                   "garden",
                                   "hiking_area",
                                   "historical_landmark",
                                   "marina",
                                   "national_park",
                                   "observation_deck",
                                   "opera_house",
                                   "park",
                                   "picnic_ground",
                                   "planetarium",
                                   "plaza",
                                   "tourist_attraction",
                                   "wildlife_park",
                                   "wildlife_refuge",
                                   "zoo"};

const size_t PLACE_TYPES_COUNT = sizeof(PLACE_TYPES) / sizeof(PLACE_TYPES[0]);

} // namespace Config

