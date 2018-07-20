BEGIN;

CREATE TABLE alembic_version (
    version_num VARCHAR(32) NOT NULL, 
    CONSTRAINT alembic_version_pkc PRIMARY KEY (version_num)
);

-- Running upgrade  -> 5aaeafd07224

CREATE TYPE halite_1_user_level AS ENUM ('High School', 'Undergraduate', 'Graduate', 'Professional');

CREATE TABLE halite_1_user (
    "userID" SERIAL NOT NULL, 
    "oauthID" INTEGER NOT NULL, 
    "oauthProvider" INTEGER NOT NULL, 
    username VARCHAR(32) NOT NULL, 
    email VARCHAR(64), 
    "isRunning" BOOLEAN DEFAULT '0' NOT NULL, 
    "compileStatus" BOOLEAN DEFAULT '0' NOT NULL, 
    organization VARCHAR(64) NOT NULL, 
    language VARCHAR(16), 
    mu FLOAT DEFAULT '25' NOT NULL, 
    sigma FLOAT DEFAULT '8.333' NOT NULL, 
    rank FLOAT, 
    "numSubmissions" INTEGER DEFAULT '0' NOT NULL, 
    "numGames" INTEGER DEFAULT '0' NOT NULL, 
    "creationTime" TIMESTAMP WITHOUT TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    "updateTime" TIMESTAMP WITHOUT TIME ZONE, 
    "onEmailList" BOOLEAN DEFAULT '1' NOT NULL, 
    "githubEmail" VARCHAR(64), 
    "verificationCode" VARCHAR(64), 
    "isEmailGood" BOOLEAN DEFAULT '0' NOT NULL, 
    level halite_1_user_level DEFAULT 'Professional' NOT NULL, 
    PRIMARY KEY ("userID")
);

CREATE TYPE organization_kind AS ENUM ('High School', 'University', 'Professional School', 'Company', 'Other');

CREATE TABLE organization (
    id SERIAL NOT NULL, 
    organization_name VARCHAR(64) NOT NULL, 
    kind organization_kind DEFAULT 'Other' NOT NULL, 
    verification_code VARCHAR(32), 
    PRIMARY KEY (id)
);

CREATE TABLE organization_email_domain (
    organization_id INTEGER NOT NULL, 
    domain VARCHAR(255) NOT NULL, 
    PRIMARY KEY (organization_id, domain), 
    CONSTRAINT organization_email_domain_ibfk_1 FOREIGN KEY(organization_id) REFERENCES organization (id) ON DELETE CASCADE
);

CREATE TYPE user_player_level AS ENUM ('High School', 'University', 'Professional');

CREATE TABLE "user" (
    id SERIAL NOT NULL, 
    oauth_id INTEGER NOT NULL, 
    oauth_provider INTEGER NOT NULL, 
    username VARCHAR(40) NOT NULL, 
    email VARCHAR(320), 
    github_email VARCHAR(320), 
    verification_code VARCHAR(64), 
    is_active BOOLEAN DEFAULT '1' NOT NULL, 
    on_email_list BOOLEAN DEFAULT '1' NOT NULL, 
    is_email_good BOOLEAN DEFAULT '0' NOT NULL, 
    player_level user_player_level DEFAULT 'Professional' NOT NULL, 
    organization_id INTEGER, 
    country_code VARCHAR(3), 
    country_subdivision_code VARCHAR(10), 
    creation_time TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    update_time TIMESTAMP WITH TIME ZONE, 
    api_key_hash VARCHAR(255), 
    is_admin BOOLEAN DEFAULT '0', 
    is_gpu_enabled BOOLEAN DEFAULT '0' NOT NULL, 
    PRIMARY KEY (id), 
    CONSTRAINT user_ibfk_1 FOREIGN KEY(organization_id) REFERENCES organization (id)
);

CREATE TYPE challenge_status AS ENUM ('created', 'playing_game', 'finished');

CREATE TABLE challenge (
    id SERIAL NOT NULL, 
    created TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL, 
    finished TIMESTAMP WITH TIME ZONE, 
    num_games INTEGER NOT NULL, 
    status challenge_status DEFAULT 'created' NOT NULL, 
    most_recent_game_task TIMESTAMP WITH TIME ZONE, 
    issuer INTEGER NOT NULL, 
    winner INTEGER, 
    PRIMARY KEY (id), 
    CONSTRAINT challenge_issuer_fk FOREIGN KEY(issuer) REFERENCES "user" (id) ON DELETE CASCADE, 
    CONSTRAINT challenge_winner_fk FOREIGN KEY(winner) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE challenge_participant (
    challenge_id INTEGER NOT NULL, 
    user_id INTEGER NOT NULL, 
    points INTEGER NOT NULL, 
    PRIMARY KEY (challenge_id, user_id), 
    CONSTRAINT challenge_participant_fk FOREIGN KEY(challenge_id) REFERENCES challenge (id) ON DELETE CASCADE, 
    CONSTRAINT challenge_participant_ibfk_2 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TYPE bot_compile_status AS ENUM ('Uploaded', 'InProgress', 'Successful', 'Failed', 'Disabled');

CREATE TABLE bot (
    user_id INTEGER NOT NULL, 
    id INTEGER NOT NULL, 
    compile_status bot_compile_status NOT NULL, 
    compile_start TIMESTAMP WITH TIME ZONE, 
    language VARCHAR(64), 
    version_number INTEGER DEFAULT '0' NOT NULL, 
    games_played INTEGER DEFAULT '0' NOT NULL, 
    mu FLOAT DEFAULT '25' NOT NULL, 
    sigma FLOAT DEFAULT '8.333' NOT NULL, 
    score FLOAT DEFAULT '0' NOT NULL, 
    creation_time TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    update_time TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    timeout_sent BOOLEAN DEFAULT '0', 
    PRIMARY KEY (user_id, id), 
    CONSTRAINT bot_ibfk_2 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE bot_history (
    user_id INTEGER NOT NULL, 
    bot_id INTEGER NOT NULL, 
    version_number INTEGER NOT NULL, 
    last_rank INTEGER NOT NULL, 
    last_score FLOAT NOT NULL, 
    last_num_players INTEGER NOT NULL, 
    last_games_played INTEGER, 
    language VARCHAR(64) NOT NULL, 
    when_retired TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    PRIMARY KEY (user_id, bot_id, version_number), 
    CONSTRAINT bot_history_ibfk_4 FOREIGN KEY(user_id, bot_id) REFERENCES bot (user_id, id) ON DELETE CASCADE, 
    CONSTRAINT bot_history_ibfk_3 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE game (
    id SERIAL NOT NULL, 
    replay_name VARCHAR(128) NOT NULL, 
    map_width INTEGER NOT NULL, 
    map_height INTEGER NOT NULL, 
    map_seed INTEGER NOT NULL, 
    map_generator VARCHAR(128) NOT NULL, 
    time_played TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    replay_bucket INTEGER DEFAULT '0' NOT NULL, 
    challenge_id INTEGER, 
    PRIMARY KEY (id), 
    CONSTRAINT game_challenge_fk FOREIGN KEY(challenge_id) REFERENCES challenge (id) ON DELETE CASCADE
);

CREATE TABLE game_stat (
    game_id INTEGER NOT NULL, 
    turns_total INTEGER NOT NULL, 
    PRIMARY KEY (game_id), 
    CONSTRAINT game_stat_ibfk_1 FOREIGN KEY(game_id) REFERENCES game (id) ON DELETE CASCADE
);

CREATE TABLE game_view_stat (
    game_id INTEGER NOT NULL, 
    views_total INTEGER NOT NULL, 
    PRIMARY KEY (game_id), 
    CONSTRAINT game_view_stat_ibfk_1 FOREIGN KEY(game_id) REFERENCES game (id) ON DELETE CASCADE
);

CREATE TABLE game_bot_stat (
    game_id INTEGER NOT NULL, 
    user_id INTEGER NOT NULL, 
    bot_id INTEGER NOT NULL, 
    PRIMARY KEY (game_id, user_id, bot_id), 
    CONSTRAINT game_bot_stat_ibfk_1 FOREIGN KEY(game_id) REFERENCES game (id) ON DELETE CASCADE, 
    CONSTRAINT fkcompid FOREIGN KEY(user_id, bot_id) REFERENCES bot (user_id, id) ON DELETE CASCADE, 
    CONSTRAINT fkuserid FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE game_participant (
    game_id INTEGER NOT NULL, 
    user_id INTEGER NOT NULL, 
    bot_id INTEGER NOT NULL, 
    version_number INTEGER NOT NULL, 
    log_name VARCHAR(256), 
    rank INTEGER NOT NULL, 
    player_index INTEGER NOT NULL, 
    timed_out BOOLEAN NOT NULL, 
    mu FLOAT, 
    sigma FLOAT, 
    leaderboard_rank INTEGER, 
    PRIMARY KEY (game_id, user_id, bot_id), 
    CONSTRAINT game_participant_ibfk_4 FOREIGN KEY(game_id) REFERENCES game (id) ON DELETE CASCADE, 
    CONSTRAINT game_participant_ibfk_3 FOREIGN KEY(user_id, bot_id) REFERENCES bot (user_id, id) ON DELETE CASCADE, 
    CONSTRAINT game_participant_ibfk_2 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE hackathon (
    id SERIAL NOT NULL, 
    title VARCHAR(256) NOT NULL, 
    description VARCHAR(4096) NOT NULL, 
    start_date TIMESTAMP WITH TIME ZONE NOT NULL, 
    end_date TIMESTAMP WITH TIME ZONE NOT NULL, 
    verification_code VARCHAR(32) NOT NULL, 
    organization_id INTEGER, 
    location VARCHAR(256), 
    thumbnail VARCHAR(512), 
    is_open BOOLEAN DEFAULT '0' NOT NULL, 
    PRIMARY KEY (id), 
    CONSTRAINT hackathon_ibfk_1 FOREIGN KEY(organization_id) REFERENCES organization (id)
);

CREATE TABLE user_tier_history (
    user_id INTEGER NOT NULL, 
    tier VARCHAR(256) NOT NULL, 
    last_in_tier TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    total_time_in_tier INTEGER DEFAULT '0', 
    PRIMARY KEY (user_id, tier), 
    CONSTRAINT user_tier_history_ibfk_2 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE hackathon_snapshot (
    hackathon_id INTEGER NOT NULL, 
    user_id INTEGER NOT NULL, 
    bot_id INTEGER NOT NULL, 
    games_played INTEGER DEFAULT '0', 
    score FLOAT NOT NULL, 
    mu FLOAT NOT NULL, 
    sigma FLOAT NOT NULL, 
    version_number INTEGER, 
    language VARCHAR(64), 
    PRIMARY KEY (hackathon_id, user_id, bot_id), 
    CONSTRAINT hackathon_snapshot_ibfk_6 FOREIGN KEY(hackathon_id) REFERENCES hackathon (id) ON DELETE CASCADE, 
    CONSTRAINT hackathon_snapshot_ibfk_5 FOREIGN KEY(user_id, bot_id) REFERENCES bot (user_id, id) ON DELETE CASCADE, 
    CONSTRAINT hackathon_snapshot_ibfk_4 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE leagues (
    id INTEGER NOT NULL, 
    category VARCHAR(45) NOT NULL, 
    name VARCHAR(45) NOT NULL, 
    description VARCHAR(1024) NOT NULL, 
    query VARCHAR(1024) NOT NULL, 
    PRIMARY KEY (id)
);

CREATE TYPE user_notification_mood AS ENUM ('error', 'neutral', 'success');

CREATE TABLE user_notification (
    id SERIAL NOT NULL, 
    user_id INTEGER NOT NULL, 
    title VARCHAR(64) NOT NULL, 
    body VARCHAR(2048) NOT NULL, 
    mood user_notification_mood DEFAULT 'neutral' NOT NULL, 
    creation_time TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    PRIMARY KEY (id), 
    CONSTRAINT user_notification_ibfk_2 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE hackathon_participant (
    hackathon_id INTEGER NOT NULL, 
    user_id INTEGER NOT NULL, 
    PRIMARY KEY (hackathon_id, user_id), 
    CONSTRAINT hackathon_participant_ibfk_4 FOREIGN KEY(hackathon_id) REFERENCES hackathon (id) ON DELETE CASCADE, 
    CONSTRAINT hackathon_participant_ibfk_3 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

CREATE TABLE badge (
    id INTEGER NOT NULL, 
    name VARCHAR(256) NOT NULL, 
    PRIMARY KEY (id)
);

CREATE TABLE user_badge (
    user_id INTEGER NOT NULL, 
    badge_id INTEGER NOT NULL, 
    is_enabled BOOLEAN DEFAULT true NOT NULL, 
    creation_time TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP, 
    update_time TIMESTAMP WITH TIME ZONE, 
    PRIMARY KEY (user_id, badge_id), 
    CONSTRAINT user_badge_ibfk_2 FOREIGN KEY(badge_id) REFERENCES badge (id) ON DELETE CASCADE, 
    CONSTRAINT user_badge_ibfk_1 FOREIGN KEY(user_id) REFERENCES "user" (id) ON DELETE CASCADE
);

INSERT INTO alembic_version (version_num) VALUES ('5aaeafd07224');

COMMIT;

