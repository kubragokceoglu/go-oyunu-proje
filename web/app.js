const API_URL = 'http://127.0.0.1:8080';
let token = null;
let myColor = null;
let currentTurn = null;
const BOARD_SIZE = 9;
let pollInterval = null;

// DOM Elements
const loginContainer = document.getElementById('login-container');
const gameContainer = document.getElementById('game-container');
const usernameInput = document.getElementById('username');
const passwordInput = document.getElementById('password');
const loginBtn = document.getElementById('login-btn');
const loginError = document.getElementById('login-error');
const goBoard = document.getElementById('go-board');
const turnIndicator = document.getElementById('turn-indicator');
const statusBadge = document.getElementById('status-badge');
const gameMessage = document.getElementById('game-message');
const passBtn = document.getElementById('pass-btn');

// Initialize Board
function initBoard() {
    goBoard.innerHTML = '';
    goBoard.style.gridTemplateColumns = `repeat(${BOARD_SIZE}, 1fr)`;
    goBoard.style.gridTemplateRows = `repeat(${BOARD_SIZE}, 1fr)`;

    for (let r = 0; r < BOARD_SIZE; r++) {
        for (let c = 0; c < BOARD_SIZE; c++) {
            const cell = document.createElement('div');
            cell.className = 'cell';
            
            // Edge classes for drawing correct lines
            if (r === 0) cell.classList.add('top');
            if (r === BOARD_SIZE - 1) cell.classList.add('bottom');
            if (c === 0) cell.classList.add('left');
            if (c === BOARD_SIZE - 1) cell.classList.add('right');

            const stone = document.createElement('div');
            stone.className = 'stone empty';
            stone.id = `stone-${r}-${c}`;
            
            cell.appendChild(stone);
            
            cell.addEventListener('click', () => {
                if (currentTurn !== myColor) {
                    showMessage("Sıra sizde değil!", true);
                    return;
                }
                playMove(r, c);
            });

            goBoard.appendChild(cell);
        }
    }
}

function showMessage(msg, isError = false) {
    gameMessage.textContent = msg;
    gameMessage.style.color = isError ? '#ef4444' : '#facc15';
    setTimeout(() => { gameMessage.textContent = ''; }, 3000);
}

// API Calls
async function login() {
    const u = usernameInput.value.trim();
    const p = passwordInput.value.trim();
    if (!u || !p) return;

    try {
        const res = await fetch(`${API_URL}/login?u=${u}&p=${p}&t=${Date.now()}`);
        const data = await res.json();
        
        if (data.status === 'ok') {
            token = data.token;
            myColor = data.color;
            
            loginContainer.classList.remove('active');
            gameContainer.classList.add('active');
            
            document.getElementById('p1-info').innerHTML = `
                <div class="stone-icon ${myColor.toLowerCase()}"></div>
                <span>${myColor === 'Black' ? 'Siyah' : 'Beyaz'} (Siz)</span>
            `;
            const oppColor = myColor === 'Black' ? 'White' : 'Black';
            document.getElementById('p2-info').innerHTML = `
                <span>${oppColor === 'Black' ? 'Siyah' : 'Beyaz'} (Rakip)</span>
                <div class="stone-icon ${oppColor.toLowerCase()}"></div>
            `;
            
            initBoard();
            pollInterval = setInterval(fetchStatus, 1000);
            fetchStatus();
        } else {
            loginError.textContent = data.msg;
        }
    } catch (err) {
        loginError.textContent = "Sunucuya bağlanılamadı. Sunucuyu başlattınız mı?";
    }
}

async function fetchStatus() {
    if (!token) return;
    try {
        const res = await fetch(`${API_URL}/status?token=${token}&t=${Date.now()}`);
        const data = await res.json();
        
        updateBoard(data.board);
        
        currentTurn = data.turn;
        turnIndicator.textContent = `Sıra: ${currentTurn === 'Black' ? 'Siyah' : 'Beyaz'}`;
        
        if (data.playersReady) {
            statusBadge.textContent = "Bağlandı";
            statusBadge.className = "status-badge ready";
        } else {
            statusBadge.textContent = "Rakip Bekleniyor...";
            statusBadge.className = "status-badge";
        }

        if (data.gameOver) {
            clearInterval(pollInterval);
            turnIndicator.textContent = "Oyun Bitti!";
            showMessage("İki oyuncu da pas geçti. Oyun sona erdi.");
        }
    } catch (err) {
        console.error("Status fetch error", err);
    }
}

async function playMove(r, c) {
    try {
        const res = await fetch(`${API_URL}/move?token=${token}&r=${r}&c=${c}&t=${Date.now()}`);
        const data = await res.json();
        if (data.status === 'error') {
            showMessage(data.msg, true);
        } else {
            fetchStatus();
        }
    } catch (err) {
        showMessage("Hamle gönderilemedi.", true);
    }
}

async function passTurn() {
    if (currentTurn !== myColor) {
        showMessage("Sıra sizde değil!", true);
        return;
    }
    try {
        const res = await fetch(`${API_URL}/pass?token=${token}&t=${Date.now()}`);
        const data = await res.json();
        if (data.status === 'ok') {
            fetchStatus();
        } else {
            showMessage(data.msg, true);
        }
    } catch (err) {
        showMessage("Pas geçilemedi.", true);
    }
}

function updateBoard(boardData) {
    for (let r = 0; r < BOARD_SIZE; r++) {
        for (let c = 0; c < BOARD_SIZE; c++) {
            const stone = document.getElementById(`stone-${r}-${c}`);
            const state = boardData[r][c];
            
            if (state === 'Black') {
                stone.className = 'stone black show';
            } else if (state === 'White') {
                stone.className = 'stone white show';
            } else {
                stone.className = 'stone empty';
            }
        }
    }
}

// Event Listeners
loginBtn.addEventListener('click', login);
passwordInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') login();
});
passBtn.addEventListener('click', passTurn);
