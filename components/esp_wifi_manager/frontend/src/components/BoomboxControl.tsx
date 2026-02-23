import { useState, useEffect } from 'preact/hooks';
import { Card } from './ui/Card';
import { Button } from './ui/Button';
import type { BoomboxStatus, RadioStation } from '../types';
import { boomboxApi } from '../api/boombox';
import './BoomboxControl.css';

const BANDS = [
  { value: 0, label: 'LW (Long Wave)' },
  { value: 1, label: 'MW (Medium Wave)' },
  { value: 2, label: 'SW (Short Wave)' },
  { value: 3, label: 'FM' },
];

const MODES = [
  { mode: 'Air', label: '📻 Air Radio', icon: '📻' },
  { mode: 'Bluetooth', label: '🔵 Bluetooth', icon: '🔵' },
  { mode: 'Web', label: '🌐 Web Radio', icon: '🌐' },
] as const;

const PLAY_ACTIONS = [
  { value: 0, label: 'Stop', icon: '⏹' },
  { value: 1, label: 'Play', icon: '▶' },
  { value: 2, label: 'Pause', icon: '⏸' },
  { value: 3, label: 'Previous', icon: '⏮' },
  { value: 4, label: 'Next', icon: '⏭' },
];

const formatAirFrequency = (frequency: number, band: string, freqRange?: string): string => {
  const normalizedBand = (band || '').trim().toUpperCase();
  const normalizedRange = (freqRange || '').trim().toUpperCase();
  const isFm = normalizedBand === 'FM' || normalizedRange === 'MHZ';

  if (isFm) {
    return (frequency / 100).toFixed(2).replace('.', ',');
  }
  return String(frequency);
};


export function BoomboxControl() {
  const [status, setStatus] = useState<BoomboxStatus | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [sendingCommand, setSendingCommand] = useState(false);

  // State for controls
  const [selectedBand, setSelectedBand] = useState(3); // FM by default
  const [frequency, setFrequency] = useState('');

  // Playlist state
  const [playlist, setPlaylist] = useState<RadioStation[]>([]);
  const [playlistLoading, setPlaylistLoading] = useState(false);
  const [newTitle, setNewTitle] = useState('');
  const [newUrl, setNewUrl] = useState('');
  const [editingIndex, setEditingIndex] = useState<number | null>(null);
  const [editingTitle, setEditingTitle] = useState('');
  const [editingUrl, setEditingUrl] = useState('');

  const loadStatus = async () => {
    try {
      const s = await boomboxApi.getStatus();
      // Normalize server-side stationIndex to 0-based for UI
      if (s && s.currentStatus && s.currentStatus.web && typeof s.currentStatus.web.stationIndex === 'number') {
        const si = s.currentStatus.web.stationIndex;
        // If server reports 1-based index, convert to 0-based
        const normalizedIndex = si > 0 ? si - 1 : si;
        const normalized = {
          ...s,
          currentStatus: {
            ...s.currentStatus,
            web: {
              ...s.currentStatus.web,
              stationIndex: normalizedIndex,
            },
          },
        };
        setStatus(normalized);
      } else {
        setStatus(s);
      }
      setError(null);
      
      // Update local state from status only on first load
      if (loading && s.currentStatus.air) {
        // Map band string to index
        const bandMap: Record<string, number> = { 'LW': 0, 'MW': 1, 'SW': 2, 'FM': 3 };
        const bandIndex = bandMap[s.currentStatus.air.band] ?? 3;
        setSelectedBand(bandIndex);
      }
    } catch (e) {
      console.error('Failed to load status:', e);
      setError((e as Error).message);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadStatus();
    const interval = setInterval(loadStatus, 2000);
    return () => clearInterval(interval);
  }, []);

  // Load playlist when switching to Web mode
  useEffect(() => {
    if (status?.currentStatus.mode === 'Web' && playlist.length === 0) {
      loadPlaylist();
    }
  }, [status?.currentStatus.mode]);

  const loadPlaylist = async () => {
    setPlaylistLoading(true);
    try {
      const data = await boomboxApi.getPlaylist();
      setPlaylist(data.stations);
    } catch (e) {
      console.error('Failed to load playlist:', e);
      setError((e as Error).message);
    } finally {
      setPlaylistLoading(false);
    }
  };

  const sendCommand = async (control: number, value: number, mode?: string) => {
    setSendingCommand(true);
    try {
      await boomboxApi.sendControl({ control, value, mode: mode as any });
      // Reload status after a short delay
      setTimeout(loadStatus, 300);
    } catch (e) {
      console.error('Failed to send command:', e);
      setError((e as Error).message);
    } finally {
      setSendingCommand(false);
    }
  };

  const handleModeChange = (mode: 'Air' | 'Bluetooth' | 'Web') => {
    // Clear status when switching modes
    setStatus((prevStatus) => {
      if (!prevStatus) return null;
      return {
        ...prevStatus,
        currentStatus: {
          hasUpdate: false,
          mode: mode,
          air: undefined,
          web: undefined,
          bluetooth: undefined,
        },
      };
    });
    sendCommand(0, 0, mode); // Send mode change command
  };

  const handleBandChange = (bandIndex: number) => {
    setSelectedBand(bandIndex);
    sendCommand(1, bandIndex); // 1 = BandIndex
  };

  const handleSeekUp = () => {
    sendCommand(10, 1); // 10 = SeekUp
  };

  const handleSeekDown = () => {
    sendCommand(11, 1); // 11 = StationStepDown (seeking down)
  };

  const handleSetFrequency = () => {
    // Поддержка формата xxx.xx или xxx,xx для FM диапазона
    const normalizedInput = frequency.replace(',', '.');
    const freq = parseFloat(normalizedInput);
    
    if (!isNaN(freq) && freq > 0) {
      // Определяем, нужно ли конвертировать (FM диапазон)
      const normalizedBand = (airStatus?.band || '').trim().toUpperCase();
      const normalizedRange = (airStatus?.freqRange || '').trim().toUpperCase();
      const isFm = normalizedBand === 'FM' || normalizedRange === 'MHZ';
      
      // Если FM и число < 200, умножаем на 100 (100.60 -> 10060)
      const freqValue = (isFm && freq < 200) ? Math.round(freq * 100) : Math.round(freq);
      
      sendCommand(16, freqValue); // 16 = SetFrequency
      setFrequency('');
    }
  };

  const handlePlayControl = (action: number) => {
    sendCommand(17, action); // 17 = PlayControl
  };

  const handleSelectStation = async (index: number) => {
    setSendingCommand(true);
    try {
      await boomboxApi.selectStation(index);

      // Optimistically update UI so selected station highlights immediately.
      setStatus((prev) => {
        if (!prev) return prev;
        return {
          ...prev,
          currentStatus: {
            ...prev.currentStatus,
            web: {
              ...prev.currentStatus.web,
              stationIndex: index,
            },
          },
        } as typeof prev;
      });

      // Refresh status from server shortly after
      setTimeout(loadStatus, 300);
    } catch (e) {
      console.error('Failed to select station:', e);
      setError((e as Error).message);
    } finally {
      setSendingCommand(false);
    }
  };

  const handleAddStation = async () => {
    if (!newUrl) return;
    setPlaylistLoading(true);
    try {
      await boomboxApi.addStation(newTitle || newUrl, newUrl);
      setNewTitle('');
      setNewUrl('');
      await loadPlaylist();
    } catch (e) {
      console.error('Failed to add station:', e);
      setError((e as Error).message);
    } finally {
      setPlaylistLoading(false);
    }
  };

  const handleDeleteStation = async (index: number) => {
    if (!confirm('Delete station?')) return;
    setPlaylistLoading(true);
    try {
      await boomboxApi.deleteStation(index);
      await loadPlaylist();
    } catch (e) {
      console.error('Failed to delete station:', e);
      setError((e as Error).message);
    } finally {
      setPlaylistLoading(false);
    }
  };

  const handleEditStation = (station: RadioStation) => {
    setEditingIndex(station.index);
    setEditingTitle(station.title || '');
    setEditingUrl(station.url || '');
  };

  const handleCancelEdit = () => {
    setEditingIndex(null);
    setEditingTitle('');
    setEditingUrl('');
  };

  const handleSaveEdit = async () => {
    if (editingIndex === null) return;
    setPlaylistLoading(true);
    try {
      await boomboxApi.updateStation(editingIndex, editingTitle || undefined, editingUrl || undefined);
      await loadPlaylist();
      handleCancelEdit();
    } catch (e) {
      console.error('Failed to update station:', e);
      setError((e as Error).message);
    } finally {
      setPlaylistLoading(false);
    }
  };

  const handleMoveStation = async (from: number, to: number) => {
    setPlaylistLoading(true);
    try {
      await boomboxApi.moveStation(from, to);
      await loadPlaylist();
    } catch (e) {
      console.error('Failed to move station:', e);
      setError((e as Error).message);
    } finally {
      setPlaylistLoading(false);
    }
  };

  if (loading) {
    return (
      <Card title="Boombox Control">
        <div class="boombox-loading">Loading...</div>
      </Card>
    );
  }

  if (error && !status) {
    return (
      <Card title="Boombox Control">
        <div class="boombox-error">
          <p>Failed to connect to Boombox</p>
          <p class="text-sm text-muted">{error}</p>
          <Button onClick={loadStatus}>Retry</Button>
        </div>
      </Card>
    );
  }

  const currentMode = status?.currentStatus.mode || 'Air';
  const airStatus = status?.currentStatus.air;
  const webStatus = status?.currentStatus.web;
  const btStatus = status?.currentStatus.bluetooth;

  return (
    <div class="boombox-container">
      {/* Mode Selection */}
      <Card title="Mode Selection">
        <div class="mode-selector">
          {MODES.map((m) => (
            <Button
              key={m.mode}
              variant={currentMode === m.mode ? 'primary' : 'secondary'}
              onClick={() => handleModeChange(m.mode)}
              disabled={sendingCommand}
            >
              <span class="mode-icon">{m.icon}</span> {m.label}
            </Button>
          ))}
        </div>
      </Card>

      {/* Status Display */}
      <Card title="Current Status">
        <div class="boombox-status">
          <div class="status-mode">
            <span class="status-label">Mode:</span>
            <span class="status-value mode-badge">{currentMode}</span>
          </div>

          {currentMode === 'Air' && airStatus && (
            <div class="air-status">
              <div class="status-row">
                <span class="status-label">Band:</span>
                <span class="status-value">{airStatus.band}</span>
              </div>
              <div class="status-row">
                <span class="status-label">Frequency:</span>
                <span class="status-value frequency">
                  {formatAirFrequency(airStatus.frequency, airStatus.band, airStatus.freqRange)} {airStatus.freqRange}
                </span>
              </div>
              <div class="status-row">
                <span class="status-label">Signal:</span>
                <span class="status-value">
                  RSSI: {airStatus.rssi} dBm • SNR: {airStatus.snr} dB
                </span>
              </div>
              <div class="status-row">
                <span class="status-label">Mode:</span>
                <span class="status-value">{airStatus.stereoMono}</span>
              </div>
              {airStatus.rds && (
                <div class="status-row">
                  <span class="status-label">RDS:</span>
                  <span class="status-value">{airStatus.rds}</span>
                </div>
              )}
            </div>
          )}

          {currentMode === 'Web' && webStatus && (
            <div class="web-status">
              <div class="status-row">
                <span class="status-label">Station:</span>
                <span class="status-value">{webStatus.station}</span>
              </div>
              {webStatus.title && (
                <div class="status-row">
                  <span class="status-label">Title:</span>
                  <span class="status-value">{webStatus.title}</span>
                </div>
              )}
              {webStatus.artist && (
                <div class="status-row">
                  <span class="status-label">Artist:</span>
                  <span class="status-value">{webStatus.artist}</span>
                </div>
              )}
              {webStatus.album && (
                <div class="status-row">
                  <span class="status-label">Album:</span>
                  <span class="status-value">{webStatus.album}</span>
                </div>
              )}
              <div class="status-row">
                <span class="status-label">URI:</span>
                <span class="status-value text-sm">{webStatus.uri}</span>
              </div>
            </div>
          )}

          {currentMode === 'Bluetooth' && btStatus && (
            <div class="bt-status">
              <div class="status-row">
                <span class="status-label">Title:</span>
                <span class="status-value">{btStatus.title}</span>
              </div>
              <div class="status-row">
                <span class="status-label">Artist:</span>
                <span class="status-value">{btStatus.artist}</span>
              </div>
              {btStatus.album && (
                <div class="status-row">
                  <span class="status-label">Album:</span>
                  <span class="status-value">{btStatus.album}</span>
                </div>
              )}
            </div>
          )}
        </div>
      </Card>

      {/* Radio Controls (Air Mode) */}
      {currentMode === 'Air' && (
        <>
          <Card title="Band Selection">
            <div class="band-selector">
              {BANDS.map((band) => (
                <Button
                  key={band.value}
                  variant={selectedBand === band.value ? 'primary' : 'secondary'}
                  onClick={() => handleBandChange(band.value)}
                  disabled={sendingCommand}
                >
                  {band.label}
                </Button>
              ))}
            </div>
          </Card>

          <Card title="Station Tuning">
            <div class="tuning-controls">
              <div class="seek-buttons">
                <Button onClick={handleSeekDown} disabled={sendingCommand}>
                  ⏪ Seek Down
                </Button>
                <Button onClick={handleSeekUp} disabled={sendingCommand}>
                  Seek Up ⏩
                </Button>
              </div>
              <div class="frequency-input">
                <input
                  type="text"
                  placeholder={airStatus?.freqRange === 'MHz' ? 'e.g. 100.60' : `Frequency in ${airStatus?.freqRange || 'kHz'}`}
                  value={frequency}
                  onInput={(e) => setFrequency((e.target as HTMLInputElement).value)}
                  onKeyDown={(e) => e.key === 'Enter' && handleSetFrequency()}
                />
                <Button onClick={handleSetFrequency} disabled={sendingCommand || !frequency}>
                  Set
                </Button>
              </div>
            </div>
          </Card>
        </>
      )}

      {/* Playback Controls for Web Radio */}
      {currentMode === 'Web' && (
        <>
          <Card title="Radio Stations">
            {playlistLoading ? (
              <div class="boombox-loading">Loading stations...</div>
            ) : playlist.length > 0 ? (
              <div class="station-list">
                {playlist.map((station, idx) => (
                  <div
                    key={station.index}
                    class={`station-item ${
                      webStatus?.stationIndex === station.index ? 'active' : ''
                    }`}
                  >
                    <div class="station-info">
                      {editingIndex === station.index ? (
                        <div>
                          <input value={editingTitle} onInput={(e) => setEditingTitle((e.target as HTMLInputElement).value)} />
                          <input value={editingUrl} onInput={(e) => setEditingUrl((e.target as HTMLInputElement).value)} />
                        </div>
                      ) : (
                        <div>
                          <div class="station-title">
                            {webStatus?.stationIndex === station.index && '▶ '}
                            {station.title}
                          </div>
                          <div class="station-url text-sm text-muted">{station.url}</div>
                        </div>
                      )}
                    </div>
                    <div style={{ display: 'flex', gap: '0.5rem' }}>
                      {editingIndex === station.index ? (
                        <>
                          <Button onClick={handleSaveEdit} disabled={playlistLoading} size="sm">Save</Button>
                          <Button onClick={handleCancelEdit} disabled={playlistLoading} size="sm">Cancel</Button>
                        </>
                      ) : (
                        <>
                          <Button
                            onClick={() => handleSelectStation(station.index)}
                            disabled={sendingCommand}
                            variant={webStatus?.stationIndex === station.index ? 'primary' : 'secondary'}
                            size="sm"
                          >
                            {webStatus?.stationIndex === station.index ? '▶ Playing' : 'Play'}
                          </Button>
                          <Button onClick={() => handleEditStation(station)} size="sm">Edit</Button>
                          <Button onClick={() => handleDeleteStation(station.index)} variant="danger" size="sm">Delete</Button>
                          <Button onClick={() => handleMoveStation(idx, Math.max(0, idx - 1))} size="sm">↑</Button>
                          <Button onClick={() => handleMoveStation(idx, Math.min(playlist.length - 1, idx + 1))} size="sm">↓</Button>
                        </>
                      )}
                    </div>
                  </div>
                ))}
              </div>
            ) : (
              <div class="text-muted">
                <p>No stations available</p>
                <Button onClick={loadPlaylist} disabled={playlistLoading}>
                  Reload Playlist
                </Button>
              </div>
            )}
          </Card>

          <Card title="Edit Playlist">
            <div class="playlist-editor">
              <input
                type="text"
                placeholder="Station title (optional)"
                value={newTitle}
                onInput={(e) => setNewTitle((e.target as HTMLInputElement).value)}
              />
              <input
                type="url"
                placeholder="Stream URL"
                value={newUrl}
                onInput={(e) => setNewUrl((e.target as HTMLInputElement).value)}
                onKeyDown={(e) => e.key === 'Enter' && handleAddStation()}
              />
              <Button onClick={handleAddStation} disabled={playlistLoading || !newUrl}>
                Add Station
              </Button>
            </div>
          </Card>

          <Card title="Playback">
            <div class="playback-controls">
              {PLAY_ACTIONS.map((action) => (
                <Button
                  key={action.value}
                  onClick={() => handlePlayControl(action.value)}
                  disabled={sendingCommand}
                  variant="secondary"
                >
                  <span class="play-icon">{action.icon}</span> {action.label}
                </Button>
              ))}
            </div>
          </Card>
        </>
      )}

      {error && (
        <div class="boombox-error-banner">
          <span>⚠️ {error}</span>
          <button onClick={() => setError(null)}>×</button>
        </div>
      )}
    </div>
  );
}
