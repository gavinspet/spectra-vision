import { useState, useEffect } from 'react'
import Head from 'next/head'
import styles from './styles.module.css'

const API_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8080'

export default function Home() {
  const [modelId, setModelId] = useState('emotion-v1')
  const [imageData, setImageData] = useState('test-image-data')
  const [response, setResponse] = useState(null)
  const [loading, setLoading] = useState(false)
  const [serverStatus, setServerStatus] = useState('checking')

  useEffect(() => {
    checkServer()
  }, [])

  const checkServer = async () => {
    try {
      const res = await fetch(`${API_URL}/health`)
      if (res.ok) {
        setServerStatus('online')
      } else {
        setServerStatus('offline')
      }
    } catch (error) {
      setServerStatus('offline')
    }
  }

  const handleEmotionDetection = async (e) => {
    e.preventDefault()
    setLoading(true)
    setResponse(null)

    try {
      const res = await fetch(`${API_URL}/api/v1/emotion`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          modelId,
          imageBase64: imageData
        })
      })

      const data = await res.json()
      setResponse({
        status: res.ok ? 'success' : 'error',
        data: JSON.stringify(data, null, 2)
      })
      setServerStatus('online')
    } catch (error) {
      setResponse({
        status: 'error',
        data: `Error: ${error.message}`
      })
      setServerStatus('offline')
    } finally {
      setLoading(false)
    }
  }

  return (
    <>
      <Head>
        <title>Spectra Vision - Emotion Detection</title>
        <meta name="description" content="Emotion detection API powered by C++" />
      </Head>

      <div className="container">
        <div className="header">
          <h1>🎭 Spectra Vision</h1>
          <p>Professional Emotion Detection API</p>
        </div>

        <div className="card">
          <h2>Emotion Detection</h2>
          
          <div className="info-box">
            <strong>API Status:</strong> {' '}
            <span className={`status ${serverStatus === 'online' ? 'online' : 'offline'}`}>
              ● {serverStatus === 'online' ? 'Online' : 'Offline'}
            </span>
            <br />
            <strong>Backend URL:</strong> <code>{API_URL}</code>
          </div>

          <form onSubmit={handleEmotionDetection}>
            <div className="form-group">
              <label htmlFor="modelId">Model ID:</label>
              <select
                id="modelId"
                value={modelId}
                onChange={(e) => setModelId(e.target.value)}
                style={{
                  width: '100%',
                  padding: '12px',
                  border: '1px solid #ddd',
                  borderRadius: '6px',
                  fontSize: '14px'
                }}
              >
                <option value="emotion-v1">emotion-v1 (Mock - Fast)</option>
                <option value="emotion-v2">emotion-v2 (Advanced)</option>
                <option value="mock">mock</option>
                <option value="advanced">advanced</option>
              </select>
            </div>

            <div className="form-group">
              <label htmlFor="imageData">Image Data (Base64 or description):</label>
              <textarea
                id="imageData"
                value={imageData}
                onChange={(e) => setImageData(e.target.value)}
                placeholder="Paste base64 encoded image or description..."
              />
            </div>

            <button type="submit" disabled={loading}>
              {loading ? 'Processing...' : 'Detect Emotion'}
            </button>
          </form>

          {loading && (
            <div className="loading show">
              <span className="spinner"></span>
              Processing...
            </div>
          )}

          {response && (
            <div className={`response show ${response.status}`}>
              <div className="response-header">
                Response ({response.status.toUpperCase()}):
              </div>
              <div className="response-content">
                {response.data}
              </div>
            </div>
          )}
        </div>

        <div className="card" style={{ background: '#f8f9fa', borderTop: '4px solid #667eea' }}>
          <h2>API Information</h2>
          <p>
            <strong>Endpoints:</strong>
          </p>
          <ul style={{ marginLeft: '20px', marginTop: '10px' }}>
            <li><code>GET /health</code> - Server status</li>
            <li><code>GET /api/v1/models</code> - List available models</li>
            <li><code>POST /api/v1/emotion</code> - Detect emotion</li>
          </ul>
          <br />
          <p>
            <strong>Response Format:</strong>
          </p>
          <pre style={{
            background: 'white',
            padding: '10px',
            borderRadius: '4px',
            overflow: 'auto'
          }}>
{`{
  "classLabel": "Happy",
  "classId": 0,
  "confidence": 0.94,
  "inferenceTimeMs": 42,
  "timestamp": "2026-07-24T15:22:44"
}`}
          </pre>
        </div>
      </div>

      <style jsx global>{`
        * {
          margin: 0;
          padding: 0;
          box-sizing: border-box;
        }

        body {
          font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
          background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
          min-height: 100vh;
          padding: 20px;
        }

        .container {
          max-width: 900px;
          margin: 0 auto;
        }

        .header {
          text-align: center;
          color: white;
          margin-bottom: 40px;
        }

        .header h1 {
          font-size: 2.5em;
          margin-bottom: 10px;
        }

        .header p {
          font-size: 1.1em;
          opacity: 0.9;
        }

        .card {
          background: white;
          border-radius: 12px;
          padding: 30px;
          margin-bottom: 20px;
          box-shadow: 0 10px 40px rgba(0, 0, 0, 0.2);
        }

        .card h2 {
          color: #667eea;
          margin-bottom: 20px;
          font-size: 1.5em;
          border-bottom: 2px solid #667eea;
          padding-bottom: 10px;
        }

        .form-group {
          margin-bottom: 15px;
        }

        label {
          display: block;
          margin-bottom: 8px;
          color: #333;
          font-weight: 500;
        }

        input[type="text"],
        textarea,
        select {
          width: 100%;
          padding: 12px;
          border: 1px solid #ddd;
          border-radius: 6px;
          font-family: 'Courier New', monospace;
          font-size: 14px;
          transition: border-color 0.3s;
        }

        input[type="text"]:focus,
        textarea:focus,
        select:focus {
          outline: none;
          border-color: #667eea;
          box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }

        textarea {
          min-height: 100px;
          resize: vertical;
        }

        button {
          background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
          color: white;
          border: none;
          padding: 12px 30px;
          border-radius: 6px;
          font-size: 14px;
          font-weight: 600;
          cursor: pointer;
          transition: transform 0.2s, box-shadow 0.2s;
        }

        button:hover:not(:disabled) {
          transform: translateY(-2px);
          box-shadow: 0 5px 20px rgba(102, 126, 234, 0.4);
        }

        button:disabled {
          opacity: 0.6;
          cursor: not-allowed;
        }

        .response {
          background: #f8f9fa;
          border-left: 4px solid #667eea;
          padding: 15px;
          border-radius: 6px;
          margin-top: 15px;
          display: none;
        }

        .response.show {
          display: block;
        }

        .response.success {
          border-left-color: #4caf50;
          background: #f1f8f4;
        }

        .response.error {
          border-left-color: #f44336;
          background: #fdf1f0;
        }

        .response-header {
          font-weight: 600;
          margin-bottom: 10px;
          color: #333;
        }

        .response-content {
          font-family: 'Courier New', monospace;
          font-size: 13px;
          color: #333;
          white-space: pre-wrap;
          word-wrap: break-word;
        }

        .status {
          display: inline-block;
          padding: 4px 12px;
          border-radius: 20px;
          font-size: 12px;
          font-weight: 600;
          margin-left: 10px;
        }

        .status.online {
          background: #e8f5e9;
          color: #2e7d32;
        }

        .status.offline {
          background: #ffebee;
          color: #c62828;
        }

        .loading {
          text-align: center;
          color: #667eea;
          display: none;
          margin-top: 15px;
        }

        .loading.show {
          display: block;
        }

        .spinner {
          display: inline-block;
          width: 16px;
          height: 16px;
          border: 2px solid #667eea;
          border-top-color: transparent;
          border-radius: 50%;
          animation: spin 0.8s linear infinite;
          margin-right: 8px;
          vertical-align: middle;
        }

        @keyframes spin {
          to { transform: rotate(360deg); }
        }

        .info-box {
          background: #f0f4ff;
          border-left: 4px solid #667eea;
          padding: 15px;
          border-radius: 6px;
          margin-bottom: 20px;
          font-size: 14px;
          color: #333;
        }

        .info-box strong {
          color: #667eea;
        }

        code {
          background: #f0f0f0;
          padding: 2px 6px;
          border-radius: 3px;
          font-family: 'Courier New', monospace;
        }

        pre {
          background: white;
          padding: 10px;
          border-radius: 4px;
          overflow: auto;
        }

        ul {
          margin-left: 20px;
          margin-top: 10px;
        }
      `}</style>
    </>
  )
}
