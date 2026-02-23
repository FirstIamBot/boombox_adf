import { useState, useEffect } from 'preact/hooks';
import { StatusCard } from './components/StatusCard';
import { NetworkList } from './components/NetworkList';
import { SavedNetworks } from './components/SavedNetworks';
import { BoomboxControl } from './components/BoomboxControl';
import { api } from './api/client';
import type { WifiStatus } from './types';
import './styles/variables.css';
import './styles/base.css';
import './styles/utilities.css';

type Tab = 'wifi' | 'boombox';

export function App() {
  const [activeTab, setActiveTab] = useState<Tab>('wifi');
  const [status, setStatus] = useState<WifiStatus | null>(null);
  const [loading, setLoading] = useState(true);

  const loadStatus = async () => {
    try {
      const s = await api.getStatus();
      setStatus(s);
    } catch (e) {
      console.error('Failed to load status:', e);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadStatus();
    const interval = setInterval(loadStatus, 5000);
    return () => clearInterval(interval);
  }, []);

  const handleConnect = () => {
    setTimeout(loadStatus, 2000);
  };

  return (
    <div class="app">
      <header class="header">
        <div class="header-content">
          <h1>ESP WiFi Manager</h1>
          {status && status.ip && (
            <div class="ip-display" title="Device IP Address">
              📡 {status.ip}
            </div>
          )}
        </div>
        <div class="tabs">
          <button
            class={`tab ${activeTab === 'wifi' ? 'active' : ''}`}
            onClick={() => setActiveTab('wifi')}
          >
            📶 WiFi
          </button>
          <button
            class={`tab ${activeTab === 'boombox' ? 'active' : ''}`}
            onClick={() => setActiveTab('boombox')}
          >
            📻 Boombox
          </button>
        </div>
      </header>

      {activeTab === 'wifi' ? (
        <>
          <StatusCard status={status} loading={loading} />
          <NetworkList onConnect={handleConnect} />
          <SavedNetworks />
        </>
      ) : (
        <BoomboxControl />
      )}
    </div>
  );
}
