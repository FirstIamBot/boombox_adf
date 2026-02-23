export interface WifiStatus {
  state: 'connected' | 'connecting' | 'disconnected';
  ssid: string;
  rssi: number;
  quality: number;
  channel: number;
  ip: string;
  netmask: string;
  gateway: string;
  dns: string;
  mac: string;
  hostname: string;
  uptime_ms: number;
  ap_active: boolean;
}

export interface ScanResult {
  ssid: string;
  rssi: number;
  auth: string;
}

export interface SavedNetwork {
  ssid: string;
  priority: number;
}

export interface APStatus {
  active: boolean;
  ssid: string;
  ip: string;
  channel: number;
  sta_count: number;
  clients: Array<{ mac: string; ip: string }>;
}

export interface APConfig {
  ssid: string;
  password: string;
  channel: number;
  max_connections: number;
  hidden: boolean;
  ip: string;
  netmask: string;
  gateway: string;
  dhcp_start: string;
  dhcp_end: string;
}

export interface Variable {
  key: string;
  value: string;
}

// Boombox types
export interface BoomboxStatus {
  lastCommand: {
    hasChanges: boolean;
    mode: string;
    control: string;
    value: number;
  };
  currentStatus: {
    hasUpdate: boolean;
    mode: 'Air' | 'Bluetooth' | 'Web';
    air?: AirStatus;
    web?: WebStatus;
    bluetooth?: BluetoothStatus;
  };
}

export interface AirStatus {
  band: string;
  stationIndex: number;
  frequency: number;
  snr: number;
  rssi: number;
  volume: number;
  freqRange: string;
  stereoMono: string;
  bandwidth: string;
  step: string;
  rds: string;
}

export interface WebStatus {
  uri: string;
  station: string;
  stationIndex: number;
  title: string;
  artist: string;
  album: string;
}

export interface BluetoothStatus {
  title: string;
  artist: string;
  album: string;
}

export interface RadioStation {
  index: number;
  title: string;
  url: string;
}

export interface PlaylistResponse {
  count: number;
  stations: RadioStation[];
}

export interface BoomboxConfig {
  mode: string;
  currentSource: number;
  volume: number;
  airConfig: {
    bandType: number;
    modulation: number;
    stepFM: number;
    stepAM: number;
    frequency: number;
    volume: number;
    bandwidthFM: number;
    bandwidthAM: number;
    bandwidthSSB: number;
    agcGain: number;
    agcEnabled: number;
    rssiThreshold: number;
    snrThreshold: number;
    fmStations: number[];
    currentFMStation: number;
  };
}

export interface BoomboxControlCommand {
  mode?: 'Air' | 'Bluetooth' | 'Web';
  control: number;
  value: number;
}

export enum BoomboxControlCode {
  BandIndex = 1,
  ModulationIndex = 2,
  StepFM = 3,
  StepAM = 4,
  BandwidthFM = 5,
  BandwidthAM = 6,
  BandwidthSSB = 7,
  StepUp = 8,
  StepDown = 9,
  SeekUp = 10,
  StationStepUp = 11,
  StationStepDown = 12,
  AGCGain = 13,
  SliderAGC = 14,
  Volume = 15,
  SetFrequency = 16,
  PlayControl = 17,
}
