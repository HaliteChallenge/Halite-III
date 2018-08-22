"""Create indices for match endpoints

Revision ID: f090029de9e3
Revises: f58f8ed53fa5
Create Date: 2018-08-22 20:09:34.906955+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import mysql


# revision identifiers, used by Alembic.
revision = 'f090029de9e3'
down_revision = 'f58f8ed53fa5'
branch_labels = None
depends_on = None


def upgrade():
    op.create_index('game_participant_user_id_index', 'game_participant', ['user_id'])
    op.create_index('game_time_played_index', 'game', ['time_played'])


def downgrade():
    op.drop_index('game_participant_user_id_index', 'game_participant')
    op.drop_index('game_time_played_index', 'game')
