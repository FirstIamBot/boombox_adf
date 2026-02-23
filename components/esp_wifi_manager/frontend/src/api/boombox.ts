import type { BoomboxStatus, BoomboxConfig, BoomboxControlCommand, PlaylistResponse } from '../types';

const BOOMBOX_BASE = '/api/boombox';

async function request<T>(path: string, options?: RequestInit): Promise<T> {
  const res = await fetch(`${BOOMBOX_BASE}${path}`, {
    ...options,
    headers: {
      'Content-Type': 'application/json',
      ...options?.headers,
    },
  });

  if (!res.ok) {
    const error = await res.json().catch(() => ({ error: res.statusText }));
    throw new Error(error.error || 'Request failed');
  }

  return res.json();
}

export const boomboxApi = {
  // Get current boombox status
  getStatus: () => request<BoomboxStatus>('/status'),

  // Get saved boombox configuration
  getConfig: () => request<BoomboxConfig>('/config'),

  // Send control command
  sendControl: (command: BoomboxControlCommand) =>
    request<{ status: string; message: string }>('/control', {
      method: 'POST',
      body: JSON.stringify(command),
    }),

  // Helper methods for common controls
  setVolume: (volume: number, mode?: 'Air' | 'Bluetooth' | 'Web') =>
    request('/control', {
      method: 'POST',
      body: JSON.stringify({ mode, control: 15, value: volume }),
    }),

  setBand: (bandType: number) =>
    request('/control', {
      method: 'POST',
      body: JSON.stringify({ control: 1, value: bandType }),
    }),

  seekUp: () =>
    request('/control', {
      method: 'POST',
      body: JSON.stringify({ control: 10, value: 1 }),
    }),

  seekDown: () =>
    request('/control', {
      method: 'POST',
      body: JSON.stringify({ control: 11, value: 1 }),
    }),

  setFrequency: (frequency: number) =>
    request('/control', {
      method: 'POST',
      body: JSON.stringify({ control: 16, value: frequency }),
    }),

  playControl: (action: 0 | 1 | 2 | 3 | 4) =>
    request('/control', {
      method: 'POST',
      body: JSON.stringify({ control: 17, value: action }),
    }),

  // Playlist management
  getPlaylist: () => request<PlaylistResponse>('/playlist'),

  selectStation: (index: number) =>
    request<{ status: string; message: string; index: number }>('/playlist/select', {
      method: 'POST',
      body: JSON.stringify({ index }),
    }),

  addStation: (title: string, url: string) =>
    request<{ status: string; message: string }>('/playlist/add', {
      method: 'POST',
      body: JSON.stringify({ title, url }),
    }),

  deleteStation: (index: number) =>
    request<{ status: string; message: string }>('/playlist/delete', {
      method: 'POST',
      body: JSON.stringify({ index }),
    }),

  updateStation: (index: number, title?: string, url?: string) =>
    request<{ status: string; message: string }>('/playlist/update', {
      method: 'POST',
      body: JSON.stringify({ index, title, url }),
    }),

  moveStation: (from: number, to: number) =>
    request<{ status: string; message: string }>('/playlist/move', {
      method: 'POST',
      body: JSON.stringify({ from, to }),
    }),
};
