import Dexie from 'dexie';

const database = new Dexie('games');

database.version(1).stores({
    games: '++id',
});

export async function addGame(participants, results) {
    try {
        if (results.final_snapshot) {
            delete results.final_snapshot;
        }
        results.participants = participants;
        results.date_played = (new Date()).toISOString();
        const key = await database.games.put(results);
        return key;
    }
    catch (e) {
        console.error('Could not add game to database', e);
        throw e;
    }
}

export async function listGames() {
    return await database.games.toArray();
}
