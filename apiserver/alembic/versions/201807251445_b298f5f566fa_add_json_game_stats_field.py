"""Add JSON game stats field

Revision ID: b298f5f566fa
Revises: 5aaeafd07224
Create Date: 2018-07-25 14:45:40.669723+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = 'b298f5f566fa'
down_revision = '5aaeafd07224'
branch_labels = None
depends_on = None


def upgrade():
    op.add_column('game',
        sa.Column('stats', postgresql.JSON()))
    op.drop_table('game_stat')
    op.drop_table('game_bot_stat')


def downgrade():
    op.create_table('game_stat',
                    sa.Column('game_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.Column('turns_total', sa.Integer(), autoincrement=False, nullable=False),
                    sa.ForeignKeyConstraint(['game_id'], ['game.id'], name='game_stat_ibfk_1', ondelete='CASCADE'),
                    sa.PrimaryKeyConstraint('game_id'),
    )
    op.create_table('game_bot_stat',
                    sa.Column('game_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.Column('user_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.Column('bot_id', sa.Integer(), autoincrement=False, nullable=False),
                    sa.ForeignKeyConstraint(['game_id'], ['game.id'], name='game_bot_stat_ibfk_1', ondelete='CASCADE'),
                    sa.ForeignKeyConstraint(['user_id', 'bot_id'], ['bot.user_id', 'bot.id'], name='fkcompid', ondelete='CASCADE'),
                    sa.ForeignKeyConstraint(['user_id'], ['user.id'], name='fkuserid', ondelete='CASCADE'),
                    sa.PrimaryKeyConstraint('game_id', 'user_id', 'bot_id'),
    )
    op.drop_column('game', 'stats')
